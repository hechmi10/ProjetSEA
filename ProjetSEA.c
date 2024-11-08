#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <curl/curl.h>

#define MAX 10  // Nombre maximum de threads

// Structure pour passer les arguments au thread
typedef struct {
    const char *url;
    const char *nom_fichier;
} TelechargementArgs;

// Fonction pour écrire les données dans un fichier
size_t ecrire_donnees(void *ptr, size_t taille, size_t nmemb, FILE *fichier) {
    return fwrite(ptr, taille, nmemb, fichier);
}

// Fonction de thread pour télécharger un fichier
void *telecharger(void *arg) {
    TelechargementArgs *args = (TelechargementArgs *)arg;

    CURL *curl;
    FILE *fichier;
    CURLcode res;

    // Initialisation de libcurl
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Erreur : Impossible d'initialiser libcurl.\n");
        pthread_exit(NULL);
    }

    // Ouvrir le fichier en écriture
    fichier = fopen(args->nom_fichier, "wb");
    if (!fichier) {
        fprintf(stderr, "Erreur : Impossible d'ouvrir le fichier %s.\n", args->nom_fichier);
        curl_easy_cleanup(curl);
        pthread_exit(NULL);
    }

    // Configurer libcurl
    curl_easy_setopt(curl, CURLOPT_URL, args->url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ecrire_donnees);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fichier);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    // Exécuter la requête
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "Erreur lors du téléchargement de %s : %s\n", args->url, curl_easy_strerror(res));
    } else {
        printf("Téléchargement terminé : %s\n", args->nom_fichier);
    }

    // Nettoyer
    fclose(fichier);
    curl_easy_cleanup(curl);

    pthread_exit(NULL);
}

int main() {
    // Liste des URLs et noms de fichiers
    const char *urls[MAX] = {
        "https://example.com/file1.txt",
        "https://example.com/file2.txt",
        "https://example.com/file3.txt",
        "https://example.com/file4.txt",
        "https://example.com/file5.txt",
        "https://example.com/file6.txt",
        "https://example.com/file7.txt",
        "https://example.com/file8.txt",
        "https://example.com/file9.txt",
        "https://example.com/file10.txt"
    };
    const char *noms_fichiers[MAX] = {
        "file1.txt", "file2.txt", "file3.txt", "file4.txt", "file5.txt",
        "file6.txt", "file7.txt", "file8.txt", "file9.txt", "file10.txt"
    };

    int i;
    pthread_t tid[MAX];
    TelechargementArgs args[MAX];

    // Créer et démarrer les threads
    for (i = 0; i < MAX; i++) {
        args[i].url = urls[i];
        args[i].nom_fichier = noms_fichiers[i];

        if (pthread_create(&tid[i], NULL, telecharger, &args[i]) != 0) {
            fprintf(stderr, "Erreur : Impossible de créer le thread %d\n", i);
            return 1;
        }
    }

    // Attendre que tous les threads se terminent
    for (i = 0; i < MAX; i++) {
        pthread_join(tid[i], NULL);
    }

    printf("Tous les téléchargements sont terminés.\n");
    return 0;
}

