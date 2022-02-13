#!/usr/bin/env bash
MODE="${MODE:=Debug}"
run_with_debugger=false

run_debugger() {
    echo 'Waiting for GDB to attach...'
    qemu-system-x86_64 \
        -S -s \
        -drive file=Distribution/i686/"${MODE}"/xyris.img,index=0,media=disk,format=raw \
        -m 4G \
        -rtc clock=host \
        -vga std \
        -serial stdio
}

run_no_debugger() {
    qemu-system-x86_64 \
        -drive file=Distribution/i686/"${MODE}"/xyris.img,index=0,media=disk,format=raw \
        -m 4G \
        -rtc clock=host \
        -vga std \
        -serial stdio \
        -monitor telnet:127.0.0.1:1234,server,nowait;
}

if ! command -v qemu-system-x86_64 > /dev/null
then
    echo 'qemu-system-x86_64 not installed'
    exit 1
fi

while getopts "d" OPTION; do
    case $OPTION in
    d)
        echo 'Attach to `qemu` with GDB by running the following commands (in GDB):'
        echo 'set arch i386:x86-64:intel'
        echo 'target remote localhost:1234'
        echo
        run_with_debugger=true
        ;;
    *)
        echo 'Incorrect options provided'
        exit 1
        ;;
    esac
done

if [ "$run_with_debugger" = true ]
then
    run_debugger
else
    run_no_debugger
fi
