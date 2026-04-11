static volatile char* const video_memory = (volatile char*)0xb8000;
static const int cols = 80;
static const int rows = 25;

static int cursor_row = 0;
static int cursor_col = 0;

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

static void terminal_putchar(char c)
{
    if (c == '\n')
    {
        cursor_col = 0;
        cursor_row++;
    }
    else
    {
        int index = (cursor_row * cols + cursor_col) *
                    2;                  // Calculate the index for the given row and column
        video_memory[index] = c;        // Character
        video_memory[index + 1] = 0x07; // Light grey on black background

        cursor_col++;
    }

    if (cursor_col >= cols)
    {
        cursor_col = 0;
        cursor_row++;
    }
    if (cursor_row >= rows)
    {
        cursor_row = 0; // Reset to the top of the screen
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
    for (int i = 0; i < rows * cols; i++)
    {
        video_memory[i * 2] = ' ';      // Clear the screen by writing spaces
        video_memory[i * 2 + 1] = 0x00; // Light grey on black background
    }
    cursor_row = 0;
    cursor_col = 0;
}

static void wait(int time)
{
    for (int i = 0; i < 100000000; i++)
    {
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

void kernel_main()
{
    serial_write_string("Kernel is starting...\n");

    clear_screen();

    for (int i = 0;; i++)
    {
        char num_buff[16];
        int_to_string(i, num_buff);
        char* output_prompt = "Current number: ";

        char output[128];
        int j = 0;
        while (output_prompt[j] != '\0')
        {
            output[j] = output_prompt[j];
            j++;
        }

        int k = 0;
        while (num_buff[k] != '\0')
        {
            output[j + k] = num_buff[k];
            k++;
        }
        output[j + k] = '\0';
        print_prompt(output);
        wait(10000000000);
    }

    serial_write_string("Kernel has finished executing.\n");
}