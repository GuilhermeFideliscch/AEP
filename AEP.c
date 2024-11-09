#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_USERS 10             // Número máximo de usuários cadastrados
#define MAX_LENGTH 20           // Tamanho máximo para nome de usuário, senha e palavra-chave
#define FILENAME "usuarios.txt" // Arquivo para salvar os dados dos usuários

struct User // Estrutura para armazenar os dados dos usuários
{
    char user[MAX_LENGTH];
    char senha[MAX_LENGTH];
    char chave_recup[MAX_LENGTH]; // Palavra-chave de recuperação
    int block;                   // Flag de bloqueio: 1 se bloqueado, 0 se não
};

int numero(char c) { //verificar os numeros da senha
    return isdigit(c);
}

int especial(char c) { //verificar os caracteres especiais da senha
    return ispunct(c);
}

int Ncaixa_alta(char c) { //verificar as letras minusculas da senha
    return islower(c); 
}

int caixa_alta(char c) { //verificar as letras maiusculas da senha
    return isupper(c);  
}

char criptografia(char c) // Funcao de criptografia
{
    if (numero(c))
    {
        return 'a' + (c - '0');
    }
    else if (especial(c))
    {
        return 'A' + (c % 26);
    }
    else if (Ncaixa_alta(c))
    {
        return '#';
    }
    else if (caixa_alta(c))
    {
        return '0';
    }
    return c;
}

void criptografia_senha(char *senh) // Funcao para criptografar a senha
{
    for (int i = 0; senh[i] != '\0'; i++)
    {
        senh[i] = criptografia(senh[i]);
    }
}

int senha_forte(const char *senh) // Funcao para verificar se a senha e forte
{
    int tamanho = strlen(senh);
    int cxAlta = 0, NcxAlta = 0, num = 0, C_especial = 0;

    if (tamanho < 8)
        return 0;

    for (int i = 0; i < tamanho; i++)
    {
        if (caixa_alta(senh[i]))
            cxAlta = 1;
        else if (Ncaixa_alta(senh[i]))
            NcxAlta = 1;
        else if (numero(senh[i]))
            num = 1;
        else if (especial(senh[i]))
            C_especial = 1;
    }

    return cxAlta && NcxAlta && num && C_especial;
}

int carregar_user(struct User users[]) // Funcao para carregar os usuarios de um arquivo
{
    FILE *file = fopen(FILENAME, "r");
    int numero_de_usuarios = 0;

    if (file == NULL)
    {
        return 0; // Arquivo nao encontrado, nao ha dados para carregar
    }

    while (fscanf(file, "%49s %49s %49s %d", users[numero_de_usuarios].user, users[numero_de_usuarios].senha, users[numero_de_usuarios].chave_recup, &users[numero_de_usuarios].block) == 4)
    {
        numero_de_usuarios++;
        if (numero_de_usuarios >= MAX_USERS)
            break;
    }

    fclose(file);
    return numero_de_usuarios;
}

void salvar_user(struct User users[], int numero_de_usuarios) // Funcao para salvar os usuarios em um arquivo
{
    FILE *file = fopen(FILENAME, "w");

    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo para salvar os dados.\n");
        return;
    }

    for (int i = 0; i < numero_de_usuarios; i++)
    {
        // Salva a senha ja criptografada, nao a criptografa novamente
        fprintf(file, "%s %s %s %d\n", users[i].user, users[i].senha, users[i].chave_recup, users[i].block);
    }

    fclose(file);
}

int login(struct User users[], int numero_de_usuarios) { // Funcao do login
    char username[MAX_LENGTH];
    char senh[MAX_LENGTH];
    int tentativas = 3;

    while (tentativas > 0) {
        printf("Digite o nome de usuario: ");
        fgets(username, sizeof(username), stdin);
        username[strcspn(username, "\n")] = 0;

        printf("Digite a senha: ");
        fgets(senh, sizeof(senh), stdin);
        senh[strcspn(senh, "\n")] = 0;

        criptografia_senha(senh);  // Encripta a senha antes de verificar

        int usuario_encontrado = 0;  // verificar se o usuário foi encontrado

        for (int i = 0; i < numero_de_usuarios; i++) {
            if (strcmp(username, users[i].user) == 0) {
                usuario_encontrado = 1;  // Usuario encontrado
                // Verifica se a conta esta bloqueada
                if (users[i].block == 1) {
                    printf("Sua conta esta bloqueada. Voce precisa alterar a senha para continuar.\n");
                    return -1;  // Conta bloqueada
                }

                // Verifica a senha do usuario
                if (strcmp(senh, users[i].senha) == 0) {
                    printf("Login bem sucedido!\n");
                    return i;  // Retorna o indice do usuario autenticado
                } else {
                    printf("Senha incorreta.\n");

                    // Pergunta ao usuario se deseja recuperar a senha
                    char opcao_de_recup;
                    printf("Deseja recuperar sua senha? (s/n): ");
                    scanf(" %c", &opcao_de_recup);
                    getchar();  // Limpa o buffer

                    if (opcao_de_recup == 's' || opcao_de_recup == 'S') {
                        mudar_senha(users, numero_de_usuarios);  // Chama a função para mudar a senha
                    }
                    break;  // Sai do loop apos a tentativa errada e possivel recuperacao
                }
            }
        }

        // Caso o nome de usuario nao seja encontrado
        if (!usuario_encontrado) {
            printf("Nome de usuario nao existe.\n");
            // Oferece ao usuario a opcao de recuperar o nome de usuario
            char opcao_de_recup;
            printf("Voce esqueceu o nome de usuario? (s/n): ");
            scanf(" %c", &opcao_de_recup);
            getchar();  // Limpa o buffer

            if (opcao_de_recup == 's' || opcao_de_recup == 'S') {
                int recuperado = recuperar_usuario(users, numero_de_usuarios);
                if (recuperado != -1) {
                    printf("Agora voce pode tentar fazer o login com o nome de usuario recuperado.\n");
                }
            }
        }

        tentativas--;
        printf("Login falhou. Tentativas restantes: %d\n", tentativas);
    }

    // Caso o usuario erre as 3 tentativas, forca a mudanca de senha
    for (int i = 0; i < numero_de_usuarios; i++) {
        if (strcmp(username, users[i].user) == 0) {
            users[i].block = 1;  // Bloqueia a conta apos 3 tentativas falhas
            printf("Conta bloqueada. Voce precisa alterar a senha para desbloquear.\n");
            return -1;
        }
    }

    return -1;
}

int registrar(struct User users[], int *numero_de_usuarios) // Funcao para cadastrar um novo usuario
{
    if (*numero_de_usuarios >= MAX_USERS)
    {
        printf("Limite de usuarios cadastrados atingido.\n");
        return 0;
    }

    char username[MAX_LENGTH];
    char senh[MAX_LENGTH];
    char chave_recup[MAX_LENGTH];

    printf("Digite um nome de usuario para cadastro: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    // Solicita a senha e verifica se atende aos requisitos
    while (1)
    {
        printf("Digite uma senha forte (minimo 8 caracteres, incluindo letra maiuscula, minuscula, numero e caractere especial): ");
        fgets(senh, sizeof(senh), stdin);
        senh[strcspn(senh, "\n")] = 0;

        if (senha_forte(senh))
        {
            criptografia_senha(senh);
            break;
        }
        else
        {
            printf("A senha nao atende aos requisitos. Tente novamente.\n");
        }
    }

    printf("Digite uma palavra-chave de recuperacao (guarde bem esse dado): ");
    fgets(chave_recup, sizeof(chave_recup), stdin);
    chave_recup[strcspn(chave_recup, "\n")] = 0;

    strcpy(users[*numero_de_usuarios].user, username);
    strcpy(users[*numero_de_usuarios].senha, senh);
    strcpy(users[*numero_de_usuarios].chave_recup, chave_recup);
    users[*numero_de_usuarios].block = 0;
    (*numero_de_usuarios)++;

    printf("Cadastro realizado com sucesso!\n");
    salvar_user(users, *numero_de_usuarios); // Salva o novo usuário imediatamente após o cadastro
    return 1;
}

int deletar_usuario(struct User users[], int *numero_de_usuarios) // Funcao para excluir a conta do usuario
{
    char username[MAX_LENGTH];
    char senh[MAX_LENGTH];
    char chave_recup[MAX_LENGTH];

    printf("Digite o nome de usuario para excluir a conta: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    printf("Digite a senha: ");
    fgets(senh, sizeof(senh), stdin);
    senh[strcspn(senh, "\n")] = 0;

    // Criptografa a senha para comparacao
    criptografia_senha(senh);

    printf("Digite a palavra chave de recuperacao: ");
    fgets(chave_recup, sizeof(chave_recup), stdin);
    chave_recup[strcspn(chave_recup, "\n")] = 0;

    // Verifica se o usuario e senha estao corretos
    for (int i = 0; i < *numero_de_usuarios; i++)
    {
        if (strcmp(username, users[i].user) == 0 && strcmp(senh, users[i].senha) == 0)
        {
            if (strcmp(chave_recup, users[i].chave_recup) == 0)
            {
                // Exclui a conta
                for (int j = i; j < (*numero_de_usuarios) - 1; j++)
                {
                    users[j] = users[j + 1];
                }
                (*numero_de_usuarios)--;
                salvar_user(users, *numero_de_usuarios);
                printf("Conta excluida com sucesso!\n");
                return 1;
            }
            else
            {
                printf("Palavra chave de recuperacao incorreta.\n");
                return 0;
            }
        }
    }

    printf("Usuario ou senha incorretos.\n");
    return 0;
}

int mudar_senha(struct User users[], int numero_de_usuarios)  // Funcao para mudar e recuperar a senha
{
    char username[MAX_LENGTH];
    char chave_recup[MAX_LENGTH];
    char nova_senha[MAX_LENGTH];

    // Solicita o nome de usuario
    printf("Digite o nome de usuario para alterar a senha: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    // Solicita a palavra chave de recuperacao
    printf("Digite a palavra-chave de recuperacao: ");
    fgets(chave_recup, sizeof(chave_recup), stdin);
    chave_recup[strcspn(chave_recup, "\n")] = 0;

    // Verifica se o usuario existe e a palavra-chave de recuperação esta correta
    for (int i = 0; i < numero_de_usuarios; i++)
    {
        if (strcmp(username, users[i].user) == 0)
        {
            if (strcmp(chave_recup, users[i].chave_recup) == 0)
            {
                // Palavra chave de recuperacao correta permite alteracao
                printf("Digite a nova senha: ");
                fgets(nova_senha, sizeof(nova_senha), stdin);
                nova_senha[strcspn(nova_senha, "\n")] = 0;

                // Verifica se a nova senha atende aos requisitos
                while (!senha_forte(nova_senha))
                {
                    printf("A senha nao atende aos requisitos. Tente novamente.\n");
                    printf("Digite a nova senha: ");
                    fgets(nova_senha, sizeof(nova_senha), stdin);
                    nova_senha[strcspn(nova_senha, "\n")] = 0;
                }

                // Criptografa a nova senha e salva
                criptografia_senha(nova_senha);
                strcpy(users[i].senha, nova_senha);
                users[i].block = 0;         // Remove o bloqueio apos a alteração de senha
                salvar_user(users, numero_de_usuarios); // Salva a nova senha e desbloqueia
                printf("Senha alterada com sucesso!\n");
                return 1; // Indica que a senha foi alterada com sucesso
            }
            else
            {
                printf("Palavra chave de recuperacao incorreta.\n");
                return 0; // Indica que a palavra chave estava errada
            }
        }
    }

    printf("Usuario nao encontrado.\n");
    return 0; // Indica que o usuario nao foi encontrado
}

int login_admin()  // Funcao para logar como admin USUARIO: admin SENHA: ACESSOaDmIn1355!@
{
    char username[MAX_LENGTH], senh[MAX_LENGTH];

    // Solicita o nome de usuario e senha para o login do admin
    printf("Digite o nome de usuario (admin): ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;

    printf("Digite a senha: ");
    fgets(senh, sizeof(senh), stdin);
    senh[strcspn(senh, "\n")] = 0;

    // Verifica se o nome de usuario e a senha sao os predefinidos
    if (strcmp(username, "admin") == 0 && strcmp(senh, "ACESSOaDmIn1355!@") == 0)
    {
        printf("Login como admin bem sucedido!\n");
        return 1; // Retorna sucesso
    }
    else
    {
        printf("Credenciais incorretas.\n");
        return 0; // Retorna falha
    }
}

int mudar_usuario(struct User users[], int numero_de_usuarios)  // Funcao para mudar o nome de usuario
{
    char usuario_antigo[MAX_LENGTH];
    char senh[MAX_LENGTH];
    char chave_recup[MAX_LENGTH];
    char novo_usuario[MAX_LENGTH];

    printf("Digite o nome de usuario atual: ");
    fgets(usuario_antigo, sizeof(usuario_antigo), stdin);
    usuario_antigo[strcspn(usuario_antigo, "\n")] = 0;

    printf("Digite a senha: ");
    fgets(senh, sizeof(senh), stdin);
    senh[strcspn(senh, "\n")] = 0;

    criptografia_senha(senh); // Criptografa a senha para comparacao

    printf("Digite a palavra chave de recuperacao: ");
    fgets(chave_recup, sizeof(chave_recup), stdin);
    chave_recup[strcspn(chave_recup, "\n")] = 0;

    // Verifica se o usuario existe e a senha esta correta
    for (int i = 0; i < numero_de_usuarios; i++)
    {
        if (strcmp(usuario_antigo, users[i].user) == 0 && strcmp(senh, users[i].senha) == 0)
        {
            if (strcmp(chave_recup, users[i].chave_recup) == 0)
            {
                // Solicita o novo nome de usuario
                printf("Digite o novo nome de usuario: ");
                fgets(novo_usuario, sizeof(novo_usuario), stdin);
                novo_usuario[strcspn(novo_usuario, "\n")] = 0;

                // Atualiza o nome de usuario
                strcpy(users[i].user, novo_usuario);

                // Salva os dados alterados
                salvar_user(users, numero_de_usuarios);
                printf("Nome de usuario alterado com sucesso!\n");
                return 1;
            }
            else
            {
                printf("Palavra chave de recuperação incorreta.\n");
                return 0;
            }
        }
    }

    printf("Usuario ou senha incorretos.\n");
    return 0;
}

int recuperar_usuario(struct User users[], int numero_de_usuarios) {  // Funcao para recuperar o nome de usuario
    char chave_recup[MAX_LENGTH];
    char senh[MAX_LENGTH];

    // Solicita a palavra chave de recuperacao
    printf("Digite sua palavra chave de recuperacao: ");
    fgets(chave_recup, sizeof(chave_recup), stdin);
    chave_recup[strcspn(chave_recup, "\n")] = 0;

    // Solicita a senha da conta
    printf("Digite a senha da sua conta: ");
    fgets(senh, sizeof(senh), stdin);
    senh[strcspn(senh, "\n")] = 0;

    criptografia_senha(senh);  // Encripta a senha antes de verificar

    // Verifica se a palavra-chave de recuperacao e a senha correspondem a um usuario
    for (int i = 0; i < numero_de_usuarios; i++) {
        if (strcmp(chave_recup, users[i].chave_recup) == 0 && strcmp(senh, users[i].senha) == 0) {
            printf("Nome de usuario encontrado: %s\n", users[i].user);
            return i;  // Retorna o indice do usuario recuperado
        }
    }

    printf("Palavra chave ou senha incorretos.\n");
    return -1;  // Caso nao encontre um usuario com as credenciais fornecidas
}

int main()  // menu
{
    
    struct User users[MAX_USERS];
    int numero_de_usuarios = carregar_user(users); // Carrega os dados dos usuarios do arquivo
    int opcao;
    int logado = -1; // Variavel para controlar o usuario logado

    while (1)
    {
        if (logado != -1)
        {
            // Se o usuario estiver logado
            printf("\nBem vindo, %s! Voce esta logado.\n", users[logado].user);
            printf("1. Sair\n");
            printf("Escolha uma opcao: ");
            scanf("%d", &opcao);
            getchar(); // Limpa o buffer do stdin

            if (opcao == 1)
            {
                salvar_user(users, numero_de_usuarios); // Salva os dados antes de sair
                printf("Saindo...\n");
                return 0; // Encerra o programa
            }
            else
            {
                printf("Opcao invalida. Tente novamente.\n");
            }
        }
        else
        {
            // Menu de opcoes
            printf("\n1. Registrar novo usuario\n");
            printf("2. Login\n");
            printf("3. Alterar senha\n");
            printf("4. Excluir conta\n");
            printf("5. Sair\n");
            printf("6. Login como Admin\n");
            printf("7. Alterar nome de usuario\n"); // Nova opcao para alterar nome de usuario
            printf("Escolha uma opcao: ");
            scanf("%d", &opcao);
            getchar(); // Limpa o buffer do stdin

            switch (opcao)
            {
            case 1:
                registrar(users, &numero_de_usuarios);
                break;
            case 2:
                logado = login(users, numero_de_usuarios); // Atualiza a variavel de usuario logado
                break;
            case 3:
                mudar_senha(users, numero_de_usuarios); // Funcao para alterar a senha
                break;
            case 4:
                deletar_usuario(users, &numero_de_usuarios);
                break;
            case 5:
                salvar_user(users, numero_de_usuarios); // Salva os dados antes de sair
                printf("Saindo...\n");
                return 0;
            case 6:
                if (login_admin())
                {
                    // Se o login for bem sucedido como admin, encerrar o programa
                    printf("Bem vindo, admin! O programa sera encerrado.\n");
                    return 0; // Encerra o programa apos login do admin
                }
                else
                {
                    printf("Falha no login como admin.\n");
                }
                break;
            case 7:
                mudar_usuario(users, numero_de_usuarios); // Chama a funcao para mudar o nome de usuario
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
            }
        }
    }
    return 0;
}