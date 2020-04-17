#include "chain.tools.hpp"

using namespace chaintools;
using namespace eosio;

std::vector <std::string> &
tools::split(const std::string &s, const std::string delim, std::vector <std::string> &result) {
    size_t last = 0;
    size_t index = s.find_first_of(delim, last);
    while (index != std::string::npos) {
        result.push_back(s.substr(last, index - last));
        last = index + 1;
        index = s.find_first_of(delim, last);
    }
    if (index - last > 0) {
        result.push_back(s.substr(last, index - last));
    }
    return result;
}

//memo的格式 method:param1,param2,param3
//method 方法 deposit
void tools::transfer(name from, name to, asset quantity, string memo) {
    require_auth(from);
    if (!(from != _self && to == _self)) {
        return;
    }
    if (from == FLASH_LOAN_CODE) {
        //TODO BUSINESS
        //TODO BUSINESS
//        check(false, "go here");

        auto pay_quantity = asset{int64_t((1 + 0.003) * quantity.amount), quantity.symbol};
        action{
                permission_level{_self, "active"_n},
                _code,
                "transfer"_n,
                std::make_tuple(_self, FLASH_LOAN_CODE, pay_quantity, std::string("repay:"))
        }.send();
        return;
    }
}

void tools::bgbulls(eosio::name player) {
    auto code = "bulls.bg"_n;
    roundinfo_t round(code, code.value);
    auto game_id = round.begin()->cur_game_id;
    auto scope = eosio::name(game_id);
    game_time_t gametime(code, code.value);
    auto it = gametime.find(game_id);
    //等待出数据
    if (it->stopbet_on_chain > 0) {
        eosio::transaction out;
        out.actions.emplace_back(
                permission_level{_self, active_permission},
                _self, "bgbulls"_n,
                make_tuple(player)
        );
        out.delay_sec = 5;
        out.send(_self.value + 2, _self, false);
        return;
    }
    eosio::transaction out;
    auto stopbet_on_chain = it->expected_stop_time;
    auto timestamp = uint64_t(now());
    check(stopbet_on_chain > timestamp, std::to_string(timestamp - stopbet_on_chain));
    out.actions.emplace_back(
            permission_level{_self, active_permission},
            _self, "testbg"_n,
            make_tuple(player, scope, game_id, timestamp)
    );
    //提前1秒执行
    out.delay_sec = stopbet_on_chain - timestamp;
    out.send(_self.value + 4, _self, false);
}

//#     id      round_id expected_stop_time stopbet_off_chain   stopbet_on_chain time
//1185	614504	39737	1586610500	1586610505	1586610506	0
void tools::testbg(eosio::name player, eosio::name scope, uint64_t game_id, uint64_t timestamp) {
    auto code = "bulls.bg"_n;
    bet_item_t bets(code, scope.value);
    for (auto it = bets.begin(); it != bets.end(); ++it) {
        if (player.value == it->player.value) {
            print(it->player.to_string() + " " + std::to_string(now()) + " " + std::to_string(timestamp));
            game_item_t gameitem(code, code.value);
            auto cur_game_it = gameitem.rbegin();
            auto sign = cur_game_it->sign;
            check(false, it->player.to_string() + sign);
            break;
        }
    }

    print("go here");
    /*action{
            permission_level{_self, active_permission},
            _self,
            "bgbulls"_n,
            std::make_tuple(player)
    }.send();*/
}

void tools::hi() {

}

void tools::test(uint64_t id, uint64_t pid, uint64_t type) {
    auto swap_newdex_code = SWAP_NEWDEX_CODE;
    pair_t pair(swap_newdex_code, swap_newdex_code.value);
    auto it = pair.find(pid);
    auto base_quantity = it->base_quantity;
    auto quote_quantity = it->quote_quantity;
    double newdex_swap_price = (quote_quantity.amount + FLOAT_SMALL) / base_quantity.amount;

    print(std::to_string(newdex_swap_price));

    if (type == TRADE_BUY) {
        buy_order_t buy_order(NEWDEX_PUBLIC_CODE, id);
        auto idx = buy_order.get_index<"byprice"_n>();
        auto begin_it = idx.rbegin();
        if (begin_it != idx.rend()) {
            double newdex_price = begin_it->price;
            print(std::to_string(newdex_price));
            //如果newdex的买单 比newdew swap价格高， 从newdew swap搬到newdex
            if (newdex_swap_price * (1 + FEE) < newdex_price) {
                auto remain_quantity = begin_it->remain_quantity;
                auto base_extend_symbol = it->base_symbol;
                auto quote_extend_symbol = it->quote_symbol;
                auto quote_contract = quote_extend_symbol.contract;
                auto base_contract = base_extend_symbol.contract;
                auto base_symbol = base_extend_symbol.symbol;
                auto swap_memo = std::string(
                        "swap-" + base_contract.to_string() + "-" + base_symbol.code().to_string());
                action{
                        permission_level{NEWDEW_PROXY_ACCOUNT, active_permission},
                        quote_contract,
                        "transfer"_n,
                        std::make_tuple(NEWDEW_PROXY_ACCOUNT, swap_newdex_code, remain_quantity, swap_memo)
                }.send();
                auto quantity = asset{int64_t((remain_quantity.amount + FLOAT_SMALL) / newdex_swap_price), base_symbol};
                auto trade_memo = std::string(
                        "{\"type\":\"sell-market\",\"symbol\":\"" + base_contract.to_string() + "-" +
                        quantity.symbol.code().to_string() +
                        "-eos\",\"price\":\"0.0000\",\"channel\":\"dapp\",\"ref\":\"tokenpocket\"}");
                print(trade_memo);
                action{
                        permission_level{NEWDEW_PROXY_ACCOUNT, active_permission},
                        base_contract,
                        "transfer"_n,
                        std::make_tuple(NEWDEW_PROXY_ACCOUNT, NEWDEX_PUBLIC_CODE, quantity, trade_memo)
                }.send();
            }
        }
    } else if (type == TRADE_SELL) {
        sell_order_t sell_order(NEWDEX_PUBLIC_CODE, id);
        auto sell_idx = sell_order.get_index<"byprice"_n>();
        auto sell_begin_it = sell_idx.begin();
        if (sell_begin_it != sell_idx.end()) {
            double newdex_price = sell_begin_it->price;
            //如果newdex的卖单 比newdew swap价格低， 从newdew搬到newdex swap
            if (newdex_swap_price > newdex_price * (1 + FEE)) {
                auto remain_quantity = sell_begin_it->remain_quantity;
                auto quote_extend_symbol = it->quote_symbol;
                auto quote_contract = quote_extend_symbol.contract;
                auto quote_symbol = quote_extend_symbol.symbol;
                auto base_extend_symbol = it->base_symbol;
                auto base_contract = base_extend_symbol.contract;
                auto base_symbol = base_extend_symbol.symbol;

                auto quantity = asset{int64_t((remain_quantity.amount + FLOAT_SMALL) * newdex_swap_price),
                                      quote_symbol};
                auto trade_memo = std::string(
                        "{\"type\":\"buy-market\",\"symbol\":\"" + base_contract.to_string() + "-" +
                        base_symbol.code().to_string() +
                        "-eos\",\"price\":\"0.0000\",\"channel\":\"dapp\",\"ref\":\"tokenpocket\"}");
                print(trade_memo);
                action{
                        permission_level{NEWDEW_PROXY_ACCOUNT, active_permission},
                        base_contract,
                        "transfer"_n,
                        std::make_tuple(NEWDEW_PROXY_ACCOUNT, NEWDEX_PUBLIC_CODE, quantity, trade_memo)
                }.send();
                check(false, quantity.to_string());
                //todo 是否需要延迟?, 不延迟 就需要池子
                auto swap_memo = std::string(
                        "swap-" + quote_contract.to_string() + "-" + quote_symbol.code().to_string());
                action{
                        permission_level{NEWDEW_PROXY_ACCOUNT, active_permission},
                        quote_contract,
                        "transfer"_n,
                        std::make_tuple(NEWDEW_PROXY_ACCOUNT, swap_newdex_code, remain_quantity, swap_memo)
                }.send();
            }
        }
    }
}

void tools::testfloan(eosio::name code, asset quantity) {
    action{
            permission_level{_self, "active"_n},
            FLASH_LOAN_CODE,
            "floan"_n,
            std::make_tuple(code, quantity, _self)
    }.send();
}

tools::~tools() {
}