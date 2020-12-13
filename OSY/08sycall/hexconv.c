/* TODO: sprintf -- convert number to hex string */

#define STDIN_FILENO    0
#define STDOUT_FILENO   1

/*
 * exit
 */
int sys_call_1(int b) {
    int ret, no = 1;
    asm volatile ("int $0x80": "=a"(ret): "a"(no), "b"(b): "memory");
    return ret;
}

/*
 * read
 */
int sys_call_3(int b, int c, int d) {
    int ret, no = 3;
    asm volatile ("int $0x80": "=a"(ret): "a"(no), "b"(b), "c"(c), "d"(d): "memory");
    return ret;
}

/*
 * write
 */
int sys_call_4(int b, int c, int d) {
    int ret, no = 4;
    asm volatile ("int $0x80": "=a"(ret): "a"(no), "b"(b), "c"(c), "d"(d): "memory");
    return ret;
}

/*
 * strlen
 */
unsigned int string_length(const char* buf) {
    unsigned int len = 0;

    while (buf[len]) {
        len++;
    }

    return len;
}

/*
 * sprintf
 */
void num_to_hex(char* buf, unsigned num) {
    char hex[17] = "0123456789abcdef";
    char temp[20];
    int index = 0;

    buf[0] = '0';
    buf[1] = 'x';

    if (num == 0) {
        buf[2] = '0';
        buf[3] = '\n';
        buf[4] = '\0';
        return;
    }

    while (num != 0) {
        temp[index++] = hex[num % 16];
        num >>= 4;
    }

    for(int i = 0; i < index; i ++) {
        buf[i + 2] = temp[index - 1 - i];
    }

    buf[(index++) + 2] = '\n';
    buf[index + 2] = '\0';
}

int isnum(char ch) {
    return ch >= '0' && ch <= '9';
}

int isspc(char ch) {
    return ch == ' ' || ch == '\n';
}

static void print(unsigned num) {
    char buf[20];
    num_to_hex(buf, num);
    int ret = sys_call_4(STDOUT_FILENO, (int)buf, string_length(buf));
    if (ret == -1) {
        sys_call_1(1);
    }
}

void _start() {
    char buf[20];
    unsigned num = 0;
    int i;
    int num_digits = 0;
    unsigned chars_in_buffer = 0;

    for (/* no init */; /* no end condition */; i++, chars_in_buffer--) {
        if (chars_in_buffer == 0) {
            int ret = sys_call_3(STDIN_FILENO, (int)buf, sizeof(buf));
            if (ret < 0) {
                sys_call_1(1);
            }
            i = 0;
            chars_in_buffer = ret;
        }

        if (num_digits > 0 && (chars_in_buffer == 0 /* EOF */ || !isnum(buf[i]))) {
            print(num);
            num_digits = 0;
            num = 0;
        }

        if (chars_in_buffer == 0 /* EOF */ || (!isspc(buf[i]) && !isnum(buf[i]))) {
            sys_call_1(0);
        }

        if (isnum(buf[i])) {
            num = num * 10 + buf[i] - '0';
            num_digits++;
        }
    }
}
