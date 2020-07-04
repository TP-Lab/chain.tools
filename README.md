# chain.tools 介绍

# chain.tools编译部署

## 依赖环境

1. eosio.cdt --- branch: release/1.5.x 或以上
2. eosio.contract --- tag:  v1.5.2 或以上

## 编译

#### 1. 将chain.tools目录放入eosio.contracts内，跟eosio.token同级即可。
#### 2. 编辑文件 eosio.contracts/CMakeLists.txt:

```
add_subdirectory(chain.tools)
```
#### 3. 运行eosio.contracts/build.sh完成编译
 ```
 ./build.sh
 ```
 
 ## 部署
```
//创建合约帐号
cleos -u 'https://eospush.tokenpocket.pro' system newaccount tp tools.tp EOS7JvShT7sTRMT9JhXUiiB4SPdqeco83e7NUSm37M2v58dvJMGr3 EOS7JvShT7sTRMT9JhXUiiB4SPdqeco83e7NUSm37M2v58dvJMGr3 --buy-ram '0.5000 EOS' --stake-net '0.1000 EOS' --stake-cpu '0.1000 EOS'
//授权
cleos -u 'https://eospush.tokenpocket.pro' set account permission tools.tp active '{"threshold": 1,"keys": [{"key": "EOS7JvShT7sTRMT9JhXUiiB4SPdqeco83e7NUSm37M2v58dvJMGr3", "weight": 1}],"accounts": [{"permission":{"actor":"tools.tp","permission":"eosio.code"},"weight":1}]}' owner -p tools.tp@owner

cleos -u 'https://eospush.tokenpocket.pro' system newaccount tp proxy.tp EOS7JvShT7sTRMT9JhXUiiB4SPdqeco83e7NUSm37M2v58dvJMGr3 EOS7JvShT7sTRMT9JhXUiiB4SPdqeco83e7NUSm37M2v58dvJMGr3 --buy-ram '0.5000 EOS' --stake-net '0.1000 EOS' --stake-cpu '0.1000 EOS'
//授权
cleos -u 'https://eospush.tokenpocket.pro' set account permission proxy.tp active '{"threshold": 1,"keys": [{"key": "EOS7JvShT7sTRMT9JhXUiiB4SPdqeco83e7NUSm37M2v58dvJMGr3", "weight": 1}],"accounts": [{"permission":{"actor":"tools.tp","permission":"eosio.code"},"weight":1}]}' owner -p proxy.tp@owner

cd build
cleos -u 'https://eospush.tokenpocket.pro' set contract tools.tp ./chain.tools -p tools.tp
```
# chain.tools测试
## 
```
cleos -u 'https://eospush.tokenpocket.pro' push action tools.tp test '[]' -p tools.tp

cleos -u 'https://eospush.tokenpocket.pro' push action tools.tp testfloan '["eosio.token", "1.0000 EOS"]' -p tools.tp
cleos -u 'https://eospush.tokenpocket.pro' push action tools.tp testfloan '["eosiotptoken", "1.0000 TPT"]' -p tools.tp
cleos -u 'https://eospush.tokenpocket.pro' push action tools.tp testfloan '["newdexissuer", "1.0000 NDX"]' -p tools.tp
```
