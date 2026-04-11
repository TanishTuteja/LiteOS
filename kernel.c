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

static void print_word(const char* word, int row, int col)
{
    char* video_memory = (char*)0xb8000;
    int cols = 80;
    int rows = 25;

    int index = (row * cols + col) * 2; // Calculate the index for the given row and column

    for (int i = 0; i < rows * cols; i++)
    {
        video_memory[i * 2] = ' ';      // Clear the screen by writing spaces
        video_memory[i * 2 + 1] = 0x00; // Light grey on black background
    }

    for (int i = 0; word[i] != '\0'; i++)
    {
        video_memory[index + i * 2] = word[i];  // Character
        video_memory[index + i * 2 + 1] = 0x07; // Light grey on black background
    }
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
    for (int i = 0; i < cols; i++)
    {
        print_word("Hello, World!", 0, i);
        wait(1000000000);
    }
    serial_write_string("Kernel has finished executing.\n");
}