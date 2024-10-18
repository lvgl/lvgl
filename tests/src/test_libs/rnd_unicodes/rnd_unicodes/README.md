# rnd_unicodes

## Description

This is a simple c lib that generates a random unicode string of a given length.

## Usage

```c
#include "library.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#define BUF_LEN 1024

utf8_t chars[BUF_LEN];

// override the random function to use the system's random number generator
uint32_t RANDOM_CALL_PROCESS(void) {
    return rand();
}

int main() {
    srand(time(NULL));
    int len = random_utf8_chars(chars, BUF_LEN, ALPHANUM_AND_CJK_TABLE, ALPHANUM_AND_CJK_TABLE_LEN, 256);

    printf("LEN: %d\n%s\n", len, chars);
    return 0;
}

```

```bash
LEN: 398
9Ky鰒9奷鱈39澂9R鴥8覗E祫f912QR梑dX啀凐覎liGPC俟j3S6V4ZBU8藉W舖旫iR8EFcHV8b尜橃o8Wsle121nb駆侓6璸9hQQ6Cy桪oc8姌f7X賣5孛0幬5Sb2埉w砏3XRSh钳D869b盓儊a95B菩62濆Z机郹9145UHaLT擎馇貤m臨v夛砃xPD7CWQq堾j稄3YmFw7Z欠P8PCcu7鶚68韾7KGgTN2P793L雙hy壒TGb螿虊C6m弩呚YBt0骇fAtn恔雸餙営n共1MeH嘿刑步焒簡RejV援21q77rT蛸4N橷4ZFa恲m詘焣悷b61瘥Z绢v
```

## License

MIT
