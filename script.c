#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "script.h"

static void flush_script_buffer(SCRIPT *script);

static char *dot_net_console_foreground_colors[] = {
    "System.ConsoleColor.Black",
    "System.ConsoleColor.DarkRed",
    "System.ConsoleColor.DarkGreen",
    "System.ConsoleColor.DarkYellow",
    "System.ConsoleColor.DarkBlue",
    "System.ConsoleColor.DarkMagenta",
    "System.ConsoleColor.DarkCyan",
    "System.ConsoleColor.Gray",
    "System.ConsoleColor.DarkGray",
    "System.ConsoleColor.Red",
    "System.ConsoleColor.Green",
    "System.ConsoleColor.Yellow",
    "System.ConsoleColor.Blue",
    "System.ConsoleColor.Magenta",
    "System.ConsoleColor.Cyan",
    "System.ConsoleColor.White"
};

SCRIPT *create_script(const char *filename) {
    SCRIPT *result = (SCRIPT *)calloc(1, sizeof(SCRIPT));
    if (result == NULL) return NULL;
    result->filename = strdup(filename);
    result->file = fopen(filename, "w");
    if (result->file == NULL) {
        free(result->filename);
        free(result);
        return NULL;
        /* TODO: Correct error handling! */
    }
    
    fprintf(result->file, "Name: Default recsh output\n");
    fprintf(result->file, "Visualizer: ConsoleVisualizer.VisConsole, ConsoleVisualizer\n");

    return result;
}

void close_script(SCRIPT *script, int extra_render_frames) {
    if (script == NULL) return;
    flush_script_buffer(script);
    script_Render(script, extra_render_frames);
    fflush(script->file);
    fclose(script->file);
    free(script->filename);
    free(script);
}

void script_Render(SCRIPT *script, const int frames) {
    if (frames) {
        flush_script_buffer(script);
        fprintf(script->file, "Render %d\n", frames);
    }
}

void script_Clear(SCRIPT *script) {
    flush_script_buffer(script);
    fprintf(script->file, "Call Clear\n");
}

void script_ResetColor(SCRIPT *script) {
    flush_script_buffer(script);
    fprintf(script->file, "Call ResetColor\n");
}

void script_Write(SCRIPT *script, const char *string) {
    flush_script_buffer(script);
    fprintf(script->file, "Call Write \"");
    while (*string) {
        switch (*string) {
            case '\n':
                fprintf(script->file, "\\n");
                break;
            case '\r':
                fprintf(script->file, "\\r");
                break;
            case '\"':
                fprintf(script->file, "\\\"");
                break;
            case '\\':
                fprintf(script->file, "\\\\");
                break;
            default:
                fprintf(script->file, "%c", *string);
        }
        ++string;
    }
    fprintf(script->file, "\"\n");
}

static void flush_script_buffer(SCRIPT *script) {
    if (script->buffer_index > 0) {
        script->buffer[script->buffer_index] = '\0';
        script->buffer_index = 0;
        script_Write(script, script->buffer);
    }
}

void script_Write_char(SCRIPT *script, const char character) {
    if (script->buffer_index == 255) {
        flush_script_buffer(script);
    }
    
    script->buffer[script->buffer_index++] = character;
}

void script_SetCursorVisible(SCRIPT *script, const int value) {
    flush_script_buffer(script);
    fprintf(script->file, "Set CursorVisible %s\n", value ? "true" : "false");
}

void script_SetCursorSize(SCRIPT *script, const int value) {
    flush_script_buffer(script);
    fprintf(script->file, "Set CursorSize %d\n", value);
}

void script_SetCursorLeft(SCRIPT *script, const int value) {
    flush_script_buffer(script);
    fprintf(script->file, "Set CursorLeft %d\n", value);
}

void script_SetCursorTop(SCRIPT *script, const int value) {
    flush_script_buffer(script);
    fprintf(script->file, "Set CursorTop %d\n", value);
}

void script_SetWindowLeft(SCRIPT *script, const int value) {
    flush_script_buffer(script);
    fprintf(script->file, "Set WindowLeft %d\n", value);
}

void script_SetWindowTop(SCRIPT *script, const int value) {
    flush_script_buffer(script);
    fprintf(script->file, "Set WindowTop %d\n", value);
}

void script_SetWindowWidth(SCRIPT *script, const int value) {
    flush_script_buffer(script);
    fprintf(script->file, "Set WindowWidth %d\n", value);
}

void script_SetWindowHeight(SCRIPT *script, const int value) {
    flush_script_buffer(script);
    fprintf(script->file, "Set WindowHeight %d\n", value);
}

void script_SetBufferWidth(SCRIPT *script, const int value) {
    flush_script_buffer(script);
    fprintf(script->file, "Set BufferWidth %d\n", value);
}

void script_SetBufferHeight(SCRIPT *script, const int value) {
    flush_script_buffer(script);
    fprintf(script->file, "Set BufferHeight %d\n", value);
}

void script_SetForegroundColor(SCRIPT *script, const int value) {
    flush_script_buffer(script);
    fprintf(script->file, "Set ForegroundColor %s\n", dot_net_console_foreground_colors[value & 15]);
}

void script_SetBackgroundColor(SCRIPT *script, const int value) {
    flush_script_buffer(script);
    fprintf(script->file, "Set BackgroundColor %s\n", dot_net_console_foreground_colors[value & 15]);
}

static void script_MoveBufferArea(SCRIPT *script, const int sourceLeft, const int sourceTop, const int sourceWidth, const int sourceHeight, const int targetLeft, const int targetTop, const char fillChar, const int fillForeground, const int fillBackground) {
    flush_script_buffer(script);
    fprintf(script->file, "Call MoveBufferArea %d, %d, %d, %d, %d, %d, '%c', %s, %s\n", sourceLeft, sourceTop, sourceWidth, sourceHeight, targetLeft, targetTop, fillChar, dot_net_console_foreground_colors[fillForeground], dot_net_console_foreground_colors[fillBackground]);
}

void script_ScrollUp(SCRIPT *script, const int width, const int height, const int foreground, const int background) {
    script_MoveBufferArea(script, 0, 1, width, height - 1, 0, 0, ' ', foreground, background);
}

void script_ScrollDown(SCRIPT *script, const int width, const int height, const int foreground, const int background) {
    script_MoveBufferArea(script, 0, 0, width, height - 1, 0, 1, ' ', foreground, background);
}
