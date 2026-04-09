void kernel_main()
{
    char* video_memory = (char*)0xb8000;
    video_memory[0] = 'A';
    video_memory[1] = 0x07; // Light grey on black background
}