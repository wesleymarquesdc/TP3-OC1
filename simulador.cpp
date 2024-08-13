#include <iostream>
#include <cstdint>
#include <iomanip>
#include <cmath>
#include <map>
#include <queue>

using namespace std;

/**
 * @brief Estrutura que representa um par de valores para a cache.
 */
struct Pair
{
  unsigned int valid; /**< Indicador de validade (1 para válido, 0 para inválido). */
  uint32_t addr;      /**< Endereço armazenado na cache. */

  /**
   * @brief Construtor padrão que inicializa a estrutura.
   */
  Pair()
  {
    addr = 0;
    valid = 0;
  }
};

/**
 * @brief Classe que implementa uma cache.
 */
class Cache
{
private:
  unsigned int miss;                              /**< Contador de misses (acessos falhos à cache). */
  unsigned int hits;                              /**< Contador de hits (acessos bem-sucedidos à cache). */
  bool lastWasMiss;                               /**< Indica se o ultimo acesso foi um miss (acesso bem-sucedido). */
  unsigned int numberOfLines;                     /**< Número total de linhas na cache. */
  unsigned int numberOfSets;                      /**< Número total de conjuntos na cache. */
  unsigned int numberOfLinesPerSet;               /**< Número de linhas por conjunto. */
  Pair *arrAddr;                                  /**< Array dinâmico que armazena os pares de endereço e validade. */
  map<unsigned int, queue<unsigned int>> mapAddr; /**< Mapa que relaciona conjuntos com suas respectivas filas de índices. */

public:
  /**
   * @brief Construtor que inicializa a cache com os parâmetros fornecidos.
   * @param numberOfLines Número total de linhas na cache.
   * @param numberOfLinesPerSet Número de linhas por conjunto.
   */
  Cache(unsigned int numberOfLines, unsigned int numberOfLinesPerSet)
  {
    this->numberOfLines = numberOfLines;
    this->numberOfLinesPerSet = numberOfLinesPerSet;
    this->numberOfSets = numberOfLines / numberOfLinesPerSet;
    this->arrAddr = new Pair[numberOfLines];
  }

  /**
   * @brief Destrutor que libera a memória alocada para a cache.
   */
  ~Cache()
  {
    delete[] arrAddr;
  }

  /**
   * @brief Retorna o número de misses ocorridos na cache.
   * @return Número de misses.
   */
  unsigned int getMiss()
  {
    return miss;
  }

  /**
   * @brief Retorna o número de hits ocorridos na cache.
   * @return Número de hits.
   */
  unsigned int getHits()
  {
    return hits;
  }

  /**
   * @brief Insere um endereço na cache.
   * @param addr Endereço a ser inserido.
   */
  void insert(uint32_t addr)
  {
    unsigned int currentSet = addr % numberOfSets;
    unsigned int indexSet = currentSet * numberOfLinesPerSet;

    // itera sobre todos os campos dentro do set para verificar se há algum endereço desoculado (valid = 0) ou se bloco já está presente
    unsigned int upperLimit = indexSet + numberOfLinesPerSet;
    for (unsigned int i = indexSet; i < upperLimit; i++)
    {
      if (!arrAddr[i].valid)
      {
        arrAddr[i].addr = addr;
        arrAddr[i].valid = 1;
        mapAddr[currentSet].push(i);
        miss++;

        display();
        lastWasMiss = 1;
        return;
      }
      if (arrAddr[i].valid && arrAddr[i].addr == addr)
      {
        // adicionar no mapa de novo? (não parece fazer sentido, pois teremos o mesmo indice duplicado, porém, foi 'adicionado novamente'...)
        hits++;

        if (lastWasMiss)
          display();
        lastWasMiss = 0;
        return;
      }
    }
    // se todos os endereços estão ocupados (valid = 1) e o bloco não está presente, ocorre substituição
    unsigned int aux = mapAddr[currentSet].front();
    mapAddr[currentSet].pop();
    arrAddr[aux].addr = addr;
    miss++;

    display();
    lastWasMiss = 1;
  }

  /**
   * @brief Exibe o estado atual da cache.
   */
  void display()
  {
    cout << "================\n";
    cout << "IDX V ** ADDR **\n";
    for (unsigned int i = 0; i < numberOfLines; i++)
    {
      cout << setw(3) << setfill('0') << i << " "; // IDX
      cout << arrAddr[i].valid << " ";             // V
      if (arrAddr[i].valid)
      {
        cout << "0x" << hex << uppercase << setw(8) << setfill('0') << arrAddr[i].addr;
      }
      cout << endl;
    }
  }
};

int main()
{
  const unsigned int cacheLength = 1024 * 4; // input (tamanho total da cache)
  const unsigned int lineLength = 1024 * 1;  // input (tamanho de cada linha)
  unsigned int numberOfLinesPerSet = 2;      // input (tamanho de cada grupo)
  const unsigned int numInput = 2;           // input

  unsigned int numberOfLines = cacheLength / lineLength;
  unsigned int offset = log2(lineLength);
  uint32_t mask = ~((1 << offset) - 1);

  uint32_t addrs[numInput], setId;
  Cache myCache = Cache(numberOfLines, numberOfLinesPerSet);

  // entrada
  for (int i = 0; i < numInput; i++)
  {
    cin >> hex >> addrs[i];
  }

  // execução
  for (int i = 0; i < numInput; i++)
  {
    setId = addrs[i] & mask;
    setId = setId >> offset;
    myCache.insert(setId);
  }
  cout << endl;

  cout << "#hits: " << myCache.getHits() << endl;
  cout << "#miss: " << myCache.getMiss() << endl;

  return 0;
}
// compila: g++ simulador.cpp -o main
// executa: ./main ou ./main < input.txt (colocando a entrada no arquivo)