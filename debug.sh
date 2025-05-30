mkdir .cache -p
valgrind --tool=massif --massif-out-file=.cache/report build/Skyo
ms_print .cache/report
