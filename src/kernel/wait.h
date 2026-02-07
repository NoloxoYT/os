void wait(int ms) {
    // On configure le timer pour qu'il batte à une certaine fréquence
    // C'est un peu complexe, mais voici la formule standard pour attendre
    for (int i = 0; i < ms; i++) {
        outb(0x43, 0x00); // Demande l'état du timer
        // On attend que la puce change d'état (boucle très courte)
        for (volatile int j = 0; j < 1000; j++); 
    }
}