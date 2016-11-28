
#include "KeyValueLib.h"
#include <stdio.h>

int main() {
    open_KeyValueDB();
    insert_elem("key", 3, "value", 5);
    char *test = get_elem("key", 3);
    remove_elem("key", 3);
}
