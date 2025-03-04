#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define IFF_TUN 0x0001
#define IFF_NO_PI 0x1000

int tun_fd;

void handle_signal(int signal) {
    if (signal == SIGINT) {
        printf("\nCerrando la interfaz TunApp y liberando recursos...\n");
        close(tun_fd);
        exit(0);
    }
}

int create_tun_interface(const char* name) {
    struct ifreq ifr;
    int fd = open("/dev/net/tun", O_RDWR);

    if (fd < 0) {
        perror("Opening /dev/net/tun");
        exit(1);
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    strncpy(ifr.ifr_name, name, IFNAMSIZ);

    if (ioctl(fd, TUNSETIFF, (void*)&ifr) < 0) {
        perror("ioctl(TUNSETIFF)");
        close(fd);
        exit(1);
    }

    return fd;
}

void configure_interface(const char* name, const char* address) {
    char command[256];
    snprintf(command, sizeof(command), "ip addr add %s dev %s", address, name);
    system(command);
    snprintf(command, sizeof(command), "ip link set dev %s up", name);
    system(command);
}

int main() {
    // Configurar el manejador de señal para SIGINT
    signal(SIGINT, handle_signal);

    tun_fd = create_tun_interface("TunApp");
    configure_interface("TunApp", "10.0.0.5/24");

    printf("TunApp interface created and configured\n");

    /*
    // Abrir el archivo para escribir el script
    FILE *file = fopen("filters.sh", "w");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }

    // Escribir el script en el archivo
    fprintf(file, "#!/bin/bash\n");
    fprintf(file, "sudo tc qdisc del dev enp2s0.9 handle ffff: ingress\n");
    fprintf(file, "sudo tc qdisc add dev enp2s0.9 handle ffff: ingress\n");
    fprintf(file, "sudo tc filter add dev enp2s0.9 parent ffff: protocol ip prio 1 flower ip_proto icmp src_ip 192.168.9.9 action mirred egress redirect dev TunApp\n");

    fclose(file);

    // Hacer el script ejecutable
    if (system("chmod +x filters.sh") == -1) {
        perror("Error al hacer ejecutable el archivo");
        return 1;
    }

    // Ejecutar el script
    if (system("./filters.sh") == -1) {
        perror("Error al ejecutar el script");
        return 1;
    }

    printf("El archivo 'filters.sh' ha sido creado y se ha ejecutado.\n");
    */

    if (system("tc qdisc del dev enp2s0.9 handle ffff: ingress") == -1) {
        perror("Error eliminando");
        return 1;
    }

    if (system("tc qdisc add dev enp2s0.9 handle ffff: ingress") == -1) {
        perror("Error añadiendo cola");
        return 1;
    }

    if (system("tc filter add dev enp2s0.9 parent ffff: protocol ip prio 1 flower ip_proto icmp src_ip 192.168.9.9 action mirred egress redirect dev TunApp") == -1) {
        perror("Error añadiendo filtro");
        return 1;
    }

    printf("Comandos introducidos\n");

    // Mantener el programa en ejecución
    while (1) {
        pause(); // Esperar una señal
    }

    return 0;
}
