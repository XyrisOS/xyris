/**
 * @file Panic.cpp
 * @author Keeton Feavel (keeton@xyr.is)
 * @brief
 * @version 0.1
 * @date 2021-11-05
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 *
 */
#include <Devices/Graphics/console.hpp>
#include <Devices/Graphics/framebuffer.hpp>
#include <Devices/Graphics/graphics.hpp>
#include <Devices/Serial/rs232.hpp>
#include <Library/stdio.hpp>
#include <Panic.hpp>
#include <Stacktrace.hpp>
#include <Scheduler/tasks.hpp>
#include <x86gprintrin.h>

#define PANIC_MAX_TRACE  64
#define PANIC_MOO_BUF_SZ 256
#define PANIC_MSG_BUF_SZ 256
#define PANIC_REG_BUG_SZ 512
#define PANIC_COLOR_FORE 0xFFFFFF
#define PANIC_COLOR_BACK 0x2D2D2D

#define PANIC_REG_DUMP_MSG "Kernel exception - Dumping register state..."

const char *funnyMessagesForMoo[] = {
    "Oh, it’s you. It’s been a long time.\nHow have you been?",
    "Sorry, I accidentally let the magic smoke out.",
    "Not sure how we got here, but here we are.",
    "Whatever you just did, don't do it again.",
    "Something udderly terrible happened!",
    "Still more stable than DOS!",
    "Yeah, that's about right.",
    "I'm Moo, the Xryis cow!",
    "Guru meditation time!",
    "Yahaha, you found me!",
    "Minecraft crashed!",
    "Get Dunked On!",
};
const size_t funnyMessagesForMooSize = sizeof(funnyMessagesForMoo) / sizeof(funnyMessagesForMoo[0]);

static void printMoo()
{
    Graphics::Framebuffer* fb = Graphics::getFramebuffer();
    Graphics::putrect(0, 0, fb->getWidth(), fb->getHeight(), PANIC_COLOR_BACK);
    Console::reset(PANIC_COLOR_FORE, PANIC_COLOR_BACK);

    char cow[PANIC_MOO_BUF_SZ];
    ksprintf(
        cow,
        "\n"
        "%s\n\n"
        "        \\   ^__^\n"
        "         \\  (OO)\\_______\n"
        "            (__)\\       )\\/\\\n"
        "             U  ||----w |\n"
        "                ||     ||\n\n",
        funnyMessagesForMoo[__rdtsc() % funnyMessagesForMooSize]);
    log_all("%s", cow);
}

static NORET void panicInternal(const char* msg, struct registers *registers)
{
    printMoo();
    if (msg) {
        log_all("%s\n\n", msg);
    }
    if (registers) {
        char buf[PANIC_REG_BUG_SZ];
        Arch::registersPrintInformation(registers);
        Arch::registersToString(buf, registers);
        log_all("%s", buf);
    }
    Stack::printTrace(PANIC_MAX_TRACE);
    Arch::haltAndCatchFire();
}

NORET void panic(const char* msg)
{
    panicInternal(msg, NULL);
}

NORET void panicf(const char* fmt, ...)
{
    va_list args;
    char buf[PANIC_MSG_BUF_SZ];
    va_start(args, fmt);
    kvsprintf(buf, fmt, args);
    va_end(args);

    panic(buf);
}

NORET void panic(struct registers *registers)
{
    panicInternal(PANIC_REG_DUMP_MSG, registers);
}
