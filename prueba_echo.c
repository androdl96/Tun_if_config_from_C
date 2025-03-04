#include <stdio.h>
#include <stdlib.h>

int main() {
    // Hacer el script ejecutable
    if (system("echo 'Este es un texto de ejemplo' > archivo_ejemplo.txt\n") == -1) {
        perror("Error al hacer ejecutable el comando");
        return 1;
    }
    printf("El archivo 'archivo_ejemplo.txt' ha sido creado y se ha añadido texto.\n");

    return 0;
}
