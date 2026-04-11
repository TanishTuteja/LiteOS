static volatile char* const video_memory = (volatile char*)0xb8000;
static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 25;

static int cursor_row = 0;
static int cursor_col = 0;

static const unsigned char KEYBOARD_LUT[128] = {
    0,    27,  '1', '2', '3',  '4', '5', '6', '7',  '8', /* 9 */
    '9',  '0', '-', '=', '\b',                           /* Backspace */
    '\t',                                                /* Tab */
    'q',  'w', 'e', 'r',                                 /* 19 */
    't',  'y', 'u', 'i', 'o',  'p', '[', ']', '\n',      /* Enter key */
    0,                                                   /* 29   - Control */
    'a',  's', 'd', 'f', 'g',  'h', 'j', 'k', 'l',  ';', /* 39 */
    '\'', '`', 0,                                        /* Left shift */
    '\\', 'z', 'x', 'c', 'v',  'b', 'n',                 /* 49 */
    'm',  ',', '.', '/', 0,                              /* Right shift */
    '*',  0,                                             /* Alt */
    ' ',                                                 /* Space bar */
    0,                                                   /* Caps lock */
    0,                                                   /* 59 - F1 key ... > */
    0,    0,   0,   0,   0,    0,   0,   0,   0,         /* < ... F10 */
    0,                                                   /* 69 - Num lock*/
    0,                                                   /* Scroll Lock */
    0,                                                   /* Home key */
    0,                                                   /* Up Arrow */
    0,                                                   /* Page Up */
    '-',  0,                                             /* Left Arrow */
    0,    0,                                             /* Right Arrow */
    '+',  0,                                             /* 79 - End key*/
    0,                                                   /* Down Arrow */
    0,                                                   /* Page Down */
    0,                                                   /* Insert Key */
    0,                                                   /* Delete Key */
    0,    0,   0,   0,                                   /* F11 Key */
    0,                                                   /* F12 Key */
    0,                                                   /* All other keys are undefined */
};

static inline void serial_write(char c)
{
    asm volatile("outb %0, %1" : : "a"(c), "Nd"(0x3F8));
}

static inline void serial_write_string(const char* str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        serial_write(str[i]);
    }
}

static int strlen(const char* str)
{
    int len = 0;
    while (str[len] != '\0')
    {
        len++;
    }
    return len;
}

static void strcpy(char* dest, const char* src)
{
    int i = 0;
    while (src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

static void strcat(char* dest, const char* src)
{
    int dest_len = strlen(dest);
    strcpy(dest + dest_len, src);
}

static void memcpy(char* dest, const char* src, int count)
{
    for (int i = 0; i < count; i++)
    {
        dest[i] = src[i];
    }
}

static void memset(char* dest, char value, int count)
{
    for (int i = 0; i < count; i++)
    {
        dest[i] = value;
    }
}

static void scroll()
{
    int src_index = VGA_WIDTH * 2;
    int dest_index = 0;
    int count = (VGA_HEIGHT - 1) * VGA_WIDTH * 2;
    memcpy(video_memory + dest_index, video_memory + src_index, count);
    for (int i = 0; i < VGA_WIDTH; i++)
    {
        video_memory[(VGA_HEIGHT - 1) * VGA_WIDTH * 2 + i * 2] = ' ';
        video_memory[(VGA_HEIGHT - 1) * VGA_WIDTH * 2 + i * 2 + 1] = 0x00;
    }
}

static void terminal_putchar(char c)
{
    if (c == '\n')
    {
        cursor_col = 0;
        cursor_row++;
    }
    else
    {
        int index = (cursor_row * VGA_WIDTH + cursor_col) *
                    2;                  // Calculate the index for the given row and column
        video_memory[index] = c;        // Character
        video_memory[index + 1] = 0x07; // Light grey on black background

        cursor_col++;
    }

    if (cursor_col >= VGA_WIDTH)
    {
        cursor_col = 0;
        cursor_row++;
    }
    if (cursor_row >= VGA_HEIGHT)
    {
        scroll();
        cursor_row = VGA_HEIGHT - 1; // Reset to the bottom of the screen
    }
}

static void terminal_write(const char* str, int length)
{
    for (int i = 0; i < length; i++)
    {
        terminal_putchar(str[i]);
    }
}

static void terminal_print(const char* str)
{
    int len = strlen(str);
    terminal_write(str, len);
}

static void terminal_print_at(const char* str, int row, int col)
{
    if (row < 0 || row >= VGA_HEIGHT || col < 0 || col >= VGA_WIDTH)
    {
        return; // Invalid position, do nothing
    }
    cursor_row = row;
    cursor_col = col;
    terminal_print(str);
}

static void print_prompt(const char* str)
{
    terminal_print("> ");
    terminal_print(str);
    terminal_print("\n");
}

static void clear_screen()
{
    for (int i = 0; i < VGA_HEIGHT * VGA_WIDTH; i++)
    {
        video_memory[i * 2] = ' ';      // Clear the screen by writing spaces
        video_memory[i * 2 + 1] = 0x00; // Light grey on black background
    }
    cursor_row = 0;
    cursor_col = 0;
}

static void wait(int time)
{
    for (int i = 0; i < time; i++)
    {
        asm volatile("" ::: "memory");
    }
}

static int int_to_string(int value, char* buffer)
{
    int i = 0;

    if (value == 0)
    {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    while (value > 0)
    {
        buffer[i++] = (value % 10) + '0'; // Convert digit to character
        value /= 10;
    }
    buffer[i] = '\0';

    // Reverse the string
    for (int j = 0; j < i / 2; j++)
    {
        char temp_char = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = temp_char;
    }
    return i;
}

static int char_to_hex(unsigned char value, char* buffer)
{
    int i = 0;

    if (value == 0)
    {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    while (value > 0)
    {
        unsigned char digit = value % 16;
        serial_write_string("Digit: ");
        buffer[i++] = digit > 9 ? digit - 10 + 'A' : digit + '0'; // Convert digit to character
        value /= 16;
    }
    buffer[i] = '\0';

    // Reverse the string
    for (int j = 0; j < i / 2; j++)
    {
        char temp_char = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = temp_char;
    }
    return i;
}

unsigned char poll_kb()
{
    unsigned char scancode;
    asm volatile("inb %1, %0" : "=a"(scancode) : "Nd"(0x60));
    return scancode;
}

unsigned char get_kb_status()
{
    unsigned char status;
    asm volatile("inb %1, %0" : "=a"(status) : "Nd"(0x64));
    return status;
}

void kernel_main()
{
    serial_write_string("Kernel is starting...\n");

    clear_screen();

    char kb_buff[256];
    int kb_buff_index = 0;

    while (1)
    {
        unsigned char status = get_kb_status();
        if ((status & 0x01) == 1)
        {
            unsigned char scancode = poll_kb();
            if (scancode & 0x80)
            {
                continue; // Ignore key release events
            }
            char scancode_hex[3];
            char_to_hex(scancode, scancode_hex);

            if (scancode == 0x1C) // Enter key
            {
                kb_buff[kb_buff_index] = '\0'; // Null-terminate the buffer
                print_prompt(kb_buff);         // Print the command prompt with the entered command
                terminal_print("\n");
                kb_buff_index = 0; // Reset buffer index for the next command
                continue;
            }
            else if (scancode == 0x0E) // Backspace key
            {
                if (kb_buff_index > 0)
                {
                    kb_buff_index--; // Move back the buffer index
                }
                terminal_print("Backspace pressed\n");
                continue;
            }

            char ascii = KEYBOARD_LUT[scancode];
            char key_pressed[2] = {ascii, '\0'};
            kb_buff[kb_buff_index++] = ascii;

            serial_write_string("Key pressed: ");
            serial_write_string(key_pressed);

            terminal_print(scancode_hex);
            terminal_print(": ");
            terminal_print(key_pressed);
            terminal_print("\n");
        }
    }

    serial_write_string("Kernel has finished executing.\n");
}