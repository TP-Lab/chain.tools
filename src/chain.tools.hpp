#ifndef WORK_CHAIN_TOOLS_H
#define WORK_CHAIN_TOOLS_H

#include <eosiolib/eosio.hpp>
#include <eosiolib/name.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>
#include <eosiolib/symbol.hpp>
#include <eosiolib/action.hpp>
#include <eosiolib/crypto.hpp>
#include <eosiolib/types.h>
#include <eosio.system/eosio.system.hpp>
#include <eosiolib/multi_index.hpp>
#include <eosiolib/permission.hpp>
#include <iterator>
#include <cstring>
#include <eosio.token/eosio.token.hpp>
#include <vector>

//#define TEST 1
#if TEST
#define EOS_CONTRACT_CODE "eosio.token"_n
#define TPT_CONTRACT_CODE "eosiotptoke1"_n
#define FLASH_LOAN_CODE "flashloan211"_n
#else
#define EOS_CONTRACT_CODE "eosio.token"_n
#define FLASH_LOAN_CODE "loan.tp"_n
#endif

#define INT_BUY_LIMIT 1
#define INT_BUY_MARKET 3
#define INT_SELL_LIMIT 2
#define INT_SELL_MARKET 4
#define NEWDEX_PUBLIC_CODE "newdexpublic"_n
#define SWAP_NEWDEX_CODE "swap.newdex"_n
#define NEWDEW_PROXY_ACCOUNT "proxy.tp"_n

#define FLOAT_SMALL 0.000001
#define FEE 0.0008
#define TRADE_BUY 0
#define TRADE_SELL 1

using namespace eosio;
using namespace std;

namespace chaintools {

    using namespace eosio;
    using eosio::name;
    using eosio::contract;
    using eosio::asset;
    using eosio::symbol_code;
    using std::string;
    using std::vector;
    using eosio::permission_level;
    using eosio::public_key;
    using eosiosystem::wait_weight;
    using eosiosystem::key_weight;
    using eosiosystem::permission_level_weight;
    using eosiosystem::authority;

    class [[eosio::contract("chain.tools")]] tools : public eosio::contract {
    public:

        using eosio::contract::contract;

        tools(eosio::name s, eosio::name code, eosio::datastream<const char *> ds) : contract(s, code, ds) {
        }

        ~tools();

        [[eosio::action]]
        void transfer(eosio::name from, eosio::name to, eosio::asset quantity, std::string memo);

        [[eosio::action]]
        void test(uint64_t id, uint64_t pid, uint64_t type);

        [[eosio::action]]
        void hi();

        [[eosio::action]]
        void testbg(eosio::name player, eosio::name scope, uint64_t game_id, uint64_t timestamp);

        [[eosio::action]]
        void bgbulls(eosio::name player);

        [[eosio::action]]
        void testfloan(eosio::name code, eosio::asset quantity);

    private:
        // code is token contract account, then scope is account
        struct [[eosio::table, eosio::contract("eosio.token")]] account {
            asset balance;

            uint64_t primary_key() const { return balance.symbol.code().raw(); }
        };

        typedef eosio::multi_index<"accounts"_n, account> accounts_table;

        struct [[eosio::table]]  game_time {
            uint64_t id;
            uint64_t round_id;
            uint64_t expected_stop_time;
            uint64_t stopbet_off_chain;
            uint64_t stopbet_on_chain;
            uint64_t time;

            uint64_t primary_key() const { return id; }

            EOSLIB_SERIALIZE( game_time, (id)(round_id)(expected_stop_time)(stopbet_off_chain)(stopbet_on_chain)(time)
            )
        };

        typedef eosio::multi_index<"gametime"_n, game_time> game_time_t;

        typedef uint8_t BYTE;

        struct [[eosio::table]]  game_item {
            uint64_t id;
            std::string sign;
            uint32_t state;
            BYTE dealer_hand_number;
            std::string dealer_card_str;
            std::string spades_card_str;
            std::string hearts_card_str;
            std::string clubs_card_str;
            std::string diamonds_card_str;
            int32_t spades_result;
            int32_t hearts_result;
            int32_t clubs_result;
            int32_t diamonds_result;
            uint64_t reveal_at;
            uint64_t created_at;
            std::string block_id;
            std::string block_hash;

            uint64_t primary_key() const { return id; }

            EOSLIB_SERIALIZE( game_item, (id)(sign)(state)(dealer_hand_number)(dealer_card_str)(spades_card_str)(
                    hearts_card_str)(clubs_card_str)(diamonds_card_str)(spades_result)(hearts_result)(clubs_result)(
                    diamonds_result)(reveal_at)(created_at)(block_id)(block_hash)
            )
        };

        typedef eosio::multi_index<"games"_n, game_item> game_item_t;

        struct [[eosio::table]]  bet_item {
            uint64_t id;
            name player;
            asset spades_ante;
            asset hearts_ante;
            asset clubs_ante;
            asset diamonds_ante;
            name proxy;
            checksum256 tx_id;
            uint32_t created_at;

            uint64_t primary_key() const { return id; }

            EOSLIB_SERIALIZE( bet_item, (id)(player)(spades_ante)(hearts_ante)(clubs_ante)(diamonds_ante)(proxy)(tx_id)(
                    created_at)
            )
        };

        typedef eosio::multi_index<"bets"_n, bet_item> bet_item_t;

        struct [[eosio::table]]  roundinfo {
            uint64_t id;
            uint64_t cur_game_id;
            asset player_investment;
            asset system_investment;
            asset player_banker_balance;
            asset system_banker_max_bet_amount;
            asset reserve_amount;
            asset leave_banker_balance;
            asset spades_ante;
            asset hearts_ante;
            asset clubs_ante;
            asset diamonds_ante;
            asset current_banker_win;
            asset current_banker_lose;
            asset banker_win_lose;
            asset player_banker_bet;
            uint32_t dispose_index;
            uint32_t state;
            uint32_t created_at;

            uint64_t primary_key() const { return id; }

            EOSLIB_SERIALIZE( roundinfo, (id)(cur_game_id)(player_investment)(system_investment)(player_banker_balance)(
                    system_banker_max_bet_amount)(reserve_amount)(leave_banker_balance)(spades_ante)(hearts_ante)(
                    clubs_ante)(diamonds_ante)(current_banker_win)(current_banker_lose)(banker_win_lose)(
                    player_banker_bet)(dispose_index)(state)(
                    created_at)
            )
        };

        typedef eosio::multi_index<"roundinfo"_n, roundinfo> roundinfo_t;

        struct wrap_symbol {
            name contract; ///< the token contract hosting the symbol
            symbol symbol; ///< the symbol

            EOSLIB_SERIALIZE( wrap_symbol, (contract)(symbol))
        };

        struct [[eosio::table]] pair {
            uint64_t pair_id;
            uint64_t sort;
            uint8_t status;
            wrap_symbol base_symbol;
            wrap_symbol quote_symbol;
            asset base_quantity;
            asset quote_quantity;
            uint64_t token;

            uint64_t primary_key() const { return pair_id; }

            EOSLIB_SERIALIZE( pair, (pair_id)(sort)(status)(base_symbol)(quote_symbol)(base_quantity)(
                    quote_quantity)(token)
            )
        };

        typedef eosio::multi_index<"pair"_n, pair> pair_t;

        struct [[eosio::table]]  order {
            uint64_t order_id;                  //order ID
            uint64_t pair_id;                    //pair ID
            uint8_t type;                          //order type, 1:limit-buy  2:limit-sell  3:market-buy  4:market-sell
            name owner;
            time_point_sec placed_time;
            asset remain_quantity;
            asset remain_convert;
            double price;
            name contract;                        // token contract
            uint8_t count;                         //transfer count
            uint8_t crosschain;                  // is crosschain
            uint64_t ext1;
            string extstr;

            uint64_t primary_key() const { return order_id; }

            uint128_t get_price() const {
                uint64_t max = 1000000000000;
                if (type == INT_BUY_LIMIT || type == INT_BUY_MARKET) {
                    uint128_t high = (uint128_t)(max * price);
                    uint64_t low = max - placed_time.utc_seconds;
                    uint128_t price128 = (high << 64) + low;
                    return price128;
                } else {
                    return (uint128_t)(max * price);
                }
            }

            uint64_t get_name() const { return owner.value; }

            EOSLIB_SERIALIZE( order, (order_id)(pair_id)(type)(owner)(placed_time)(remain_quantity)(remain_convert)(
                    price)(contract)(count)(crosschain)(ext1)(extstr)
            )
        };

        typedef eosio::multi_index<"buyorder"_n, order,
                indexed_by<"byprice"_n, const_mem_fun < order, uint128_t, &order::get_price> >,
        indexed_by<"byname"_n, const_mem_fun<order, uint64_t, &order::get_name> > >
        buy_order_t;

        typedef eosio::multi_index<"sellorder"_n, order,
                indexed_by<"byprice"_n, const_mem_fun < order, uint128_t, &order::get_price> >,
        indexed_by<"byname"_n, const_mem_fun<order, uint64_t, &order::get_name> > >
        sell_order_t;
    private:
        static std::vector <std::string> &
        split(const std::string &s, const std::string delim, std::vector <std::string> &result);

        static uint128_t _ttoi(eosio::name code, eosio::symbol symbol) {
            return (uint128_t(code.value) << 64) + symbol.raw();
        }

        static constexpr eosio::name active_permission{"active"_n};

    private:
        uint32_t delay_sec = 1;
    };
}

#define EOSIO_DISPATCH_EX(TYPE, MEMBERS) \
            extern "C" { \
                void apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
                    auto self = receiver; \
                    if( action == eosio::name("onerror").value) { \
                        /* onerror is only valid if it is for the "enumivo" code account and authorized by "eosio"'s "active permission */ \
                        eosio_assert(code == eosio::name("eosio").value, "onerror action's are only valid from the \"eosio\" system account"); \
                    } \
                    if((code == self && action != eosio::name("transfer").value) || action == eosio::name("onerror").value || (action == eosio::name("transfer").value && code != self)) { \
                            switch( action ) { \
                            EOSIO_DISPATCH_HELPER( TYPE, MEMBERS ) \
                        } \
                    } \
                } \
            } \

EOSIO_DISPATCH_EX(chaintools::tools, (transfer)(test)(testbg)(bgbulls)(hi)(testfloan))

#endif //WORK_CHAIN_TOOLS_H
