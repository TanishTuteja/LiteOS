static char* const video_memory = (const char*)0xb8000;
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

static void print_char(char c)
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

static void print(const char* str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        print_char(str[i]);
    }
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

void kernel_main()
{
    int cols = 80;
    serial_write_string("Kernel is starting...\n");

    clear_screen();

    while (1)
    {
        print("Hello, World! ");
        wait(1000000000);
    }

    serial_write_string("Kernel has finished executing.\n");
}