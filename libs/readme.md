# 1. openssl
```
./Configure
make
```
Notes
- Mya need to run `dos2unix` on `Configure`

# 2. libev
```
./autogen.sh
./configure
make
```

# 3. amqp-cpp
```
cmake -DAMQP-CPP_LINUX_TCP=ON -DOPENSSL_ROOT_DIR=/rl/libs/openssl/ -DOPENSSL_INCLUDE_DIR=/rl/libs/openssl/include -DOPENSSL_LIBRARIES=/rl/libs/openssl -DOPENSSL_CRYPTO_LIBRARY=/rl/libs/openssl -DOPENSSL_USE_STATIC_LIBS=ON ..
make

```


If that fails, try adding to `CMakeLists.txt`:
```
set(OPENSSL_ROOT_DIR "/rl/libs/openssl")
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
```
```
option(AMQP-CPP_LINUX_TCP "Build linux sockets implementation." ON)
```

# 4. cereal
cd