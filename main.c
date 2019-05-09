#include <math.h>
#include <string.h>		// para usar strings

// Rotinas para acesso da OpenGL
#include "opengl.h"

// Protótipos
void process();
void carregaHeader(FILE* fp);
void carregaImagem(FILE* fp, int largura, int altura);
void criaImagensTeste();


// Dimensões da imagem de entrada
int sizeX, sizeY;

// Header da imagem de entrada
unsigned char header[11];

// Pixels da imagem de ENTRADA (em formato RGBE)
unsigned char* image;

// Pixels da imagem de SAÍDA (em formato RGB)
unsigned char* image8;

// Fator de exposição inicial
float exposure = 1.0;

// Função principal de processamento: ela deve chamar outras funções
// quando for necessário (ex: algoritmo de tone mapping, etc)
void process()
{
    printf("Exposure: %.3f\n", exposure);
    //
    // EXEMPLO: preenche a imagem com pixels cor de laranja...
    //
    int totalPixels = (sizeX * sizeY)*4;
    unsigned char* ptrOrgImg = image;

    unsigned int R;
    unsigned int G;
    unsigned int B;
    unsigned int m;
    float* Rptr;
    float* Gptr;
    float* Bptr;
    Rptr = malloc(sizeof(float) * (totalPixels/4));
    Gptr = malloc(sizeof(float) * (totalPixels/4));
    Bptr = malloc(sizeof(float) * (totalPixels/4));

    int count = 0;
    for (int i = 0; i < totalPixels; i+=4)
    //for (int i = 0; i < 32; i+=4)
    {


        R = image[i];
        G = image[i+1];
        B = image[i+2];
        m = image[i+3];
        //printf("RGB m: %02X %02X %02X %02X", R, G, B, m);


        int pot = m - 136;
        float c = pow(2, pot );
        //printf("\n valor do c: %f ", c);
        *Rptr++ = R * c;
        *Gptr++ = G * c;
        *Bptr++ = B * c;
        count ++;
        //printf("\n RGB (%f,%f,%f)", f_r, f_g , f_b);
    }

    Rptr -= count;
    Gptr -= count;
    Bptr -= count;

    
  
    unsigned char* ptr = image8;
    int totalBytes = sizeX * sizeY * 3; // RGB = 3 bytes por pixel
    for(int pos=0; pos<totalBytes; pos+=3) 
    {
        *ptr++ = (unsigned char) (((*Rptr * exposure)/((*Rptr++ * exposure)+0.5))*255);
        *ptr++ = (unsigned char) (((*Gptr * exposure)/((*Gptr++ * exposure)+0.5))*255);
        *ptr++ = (unsigned char) (((*Bptr * exposure)/((*Bptr++ * exposure)+0.5))*255);
    }
    ptr = image8;
    // for(int k=0; k<totalBytes; k+=3)
    // {
    //     *ptr++ = (unsigned char) ((*ptr) / ((*ptr)+0.5));
    //     *ptr++ = (unsigned char) ((*ptr) / ((*ptr)+0.5));
    //     *ptr++ = (unsigned char) ((*ptr) / ((*ptr)+0.5));
    // }

    //
    // NÃO ALTERAR A PARTIR DAQUI!!!!
    //
    buildTex();
}


// Função apenas para a criação de uma imagem em memória, com o objetivo
// de testar a funcionalidade de exibição e controle de exposição do programa
void criaImagensTeste()
{
    // TESTE: cria uma imagem de 800x600
    sizeX = 800;
    sizeY = 600;

    printf("%d x %d\n", sizeX, sizeY);

    // Aloca imagem de entrada (32 bits RGBE)
    image = (unsigned char*) malloc(sizeof(unsigned char) * sizeX * sizeY * 4);

    // Aloca memória para imagem de saída (24 bits RGB)
    image8 = (unsigned char*) malloc(sizeof(unsigned char) * sizeX * sizeY * 3);
}

// Esta função deverá ser utilizada para ler o conteúdo do header
// para a variável header (depois você precisa extrair a largura e altura da imagem desse vetor)
void carregaHeader(FILE* fp)
{
    // Lê 11 bytes do início do arquivo
    fread(header, 11, 1, fp);
    // Exibe os 3 primeiros caracteres, para verificar se a leitura ocorreu corretamente
    printf("Id: %c%c%c\n", header[0], header[1], header[2]);
}

// Esta função deverá ser utilizada para carregar o restante
// da imagem (após ler o header e extrair a largura e altura corretamente)
void carregaImagem(FILE* fp, int largura, int altura)
{
    sizeX = largura;
    sizeY = altura;

    // Aloca imagem de entrada (32 bits RGBE)
    image = (unsigned char*) malloc(sizeof(unsigned char) * sizeX * sizeY * 4);

    // Aloca memória para imagem de saída (24 bits RGB)
    image8 = (unsigned char*) malloc(sizeof(unsigned char) * sizeX * sizeY * 3);

    // Lê o restante da imagem de entrada
    fread(image, sizeX * sizeY * 4, 1, fp);
    // Exibe primeiros 3 pixels, para verificação
    for(int i=0; i<12; i+=4) {
        printf("%02X %02X %02X %02X\n", image[i], image[i+1], image[i+2], image[i+3]);
    }
}

int main(int argc, char** argv)
{
    if(argc==1) {
        printf("hdrvis [image file.hdf]\n");
        exit(1);
    }
    // Inicialização da janela gráfica
    init(argc,argv);

    FILE* arq = fopen(argv[1], "rb");
    carregaHeader(arq);

    printf("HDR: ");
    for(int i=0; i<11; i++)
    {
        printf(" %u ", header[i]);
    }
    printf("\n");

    // Cálculo da largura da imagem em pixels
    unsigned long int width = 0;
    width = (header[6] << 24) | (header[5] << 16) | (header[4] << 8) | (header[3]);
    printf("width: %u \n", width);

    // Cálculo da altura da imagem em pixels
    unsigned int height = 0;
    height = (header[10] << 24) | (header[9] << 16) | (header[8] << 8) | (header[7]);
    printf("height: %u \n", height);

    carregaImagem(arq, width, height);

    // Fecha o arquivo
    fclose(arq);

    //
    // COMENTE a linha abaixo quando a leitura estiver funcionando!
    // (caso contrário, ele irá sobrepor a imagem carrega com a imagem de teste)
    //
    //criaImagensTeste();

    exposure = 1.0f; // exposição inicial

    // Aplica processamento inicial
    process();

    // Não retorna... a partir daqui, interação via teclado e mouse apenas, na janela gráfica

    // Mouse wheel é usada para aproximar/afastar
    // Setas direita e esquerda ajustam o fator de exposição
   
    glutMainLoop();

    return 0;
}