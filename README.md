# lsm-kv
## Introduction
KV db using lsm tree in cpp, temporally in synchronous manner when using getting and  inserting.

usage & smoke test:
```bash
cd /path/to/lsm-kv
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

if for debug use this cmds:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```
Debug mode will provide more detailed logs to help you to debug.

## Future work
* Robustness
    * WAL
    * log support
* Clean Code
    * pair -> struct
    * unit test
* Performance
    * async merge
    * async operation
    * bloom filter
    * query cache
    * restart point (compressing)
* misc
    * flexiable configuration 