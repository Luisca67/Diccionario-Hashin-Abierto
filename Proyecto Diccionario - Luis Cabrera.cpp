#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>
#include <limits>

using std::cout;
using std::cin;
using std::string;
using std::ifstream;
using std::ofstream;

const int HashSize{26};

struct node final{
	string m_palabra;
	string m_significado;
	node* next;
	node(string word = "", string meaning = ""): m_palabra(word), m_significado(meaning), next(nullptr) { }
};

/* Sypnosis: CLASE HASHMAP */
class HashMap final{
		const string orden = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		node* hashing[HashSize];
		
		int hash(node*);
		int hash(string);
		int cmp(string, string);
		void sort(node*, string, string);
		string to_upper(string);
		void caseLetter(string&);
	public:
		HashMap();
		bool insert(node*);
		bool modify(string, string, string);
		bool remove(string, string);
		bool belongs(string);
		void def(string);
		void print(string);
		bool unPackFile(string);
		bool packingFile();
};

HashMap::HashMap() {
	for(int i{0}; i < HashSize; ++i) {
		hashing[i] = nullptr;
	}
}

//METODO PRIVADO: VOLVER A MAYUSCULA EL CARACTER
string HashMap::to_upper(string n) {
	std::transform(n.begin(), n.end(), n.begin(), ::toupper);
	return n;
}
//METODO PRIVADO: ASEGURA PRIMERA LETRA EN MAYUSCULA
void HashMap::caseLetter(string& s) {
	string aux = "";
	aux += s[0];
	aux = to_upper(aux);
	for(int i{1}; i < static_cast<int>(s.length()); ++i)
		aux += s[i];
		
	s = aux;
}
//METODO PRIVADO: DECIDE LUGAR DE LA LETRA
int HashMap::hash(node* n) {
	for(int i{0}; i < HashSize; ++i)
		if(n->m_palabra[0] == orden[i])
			return i;
			
	return -1;
}
//METODO PRIVADO: DECIDE LUGAR A BUSCAR, POR LETRA
int HashMap::hash(string n) {
	for(int i{0}; i < HashSize; ++i)
		if(n[0] == orden[i])
			return i;
	
	if(n == "#") return -5; //Ayuda a indicar que se imprima el diccionario
	
	return -1;
}
//METODO PRIVADO: COMPARAR PALABRAS EN FORMATO STRING
int HashMap::cmp(string a, string b) {
	int x = a.length(), y = b.length();
	bool chg{ false };
	if(x > y) {
		chg = true;
		int t{x};
		x = y;
		y = t;
	}
	
	for(int i{0}; i < x; ++i) {
		for(int j{i}; j < i+1; ++j) {
			const char w[2]{a[i], '\0'}, t[2]{b[j], '\0'};
			if(std::strcmp(w, t) > 0) {
				return 1;
			} else if(std::strcmp(w, t) < 0){
				return -1;
			}
		}
	}
	if(chg == true) return -2; //Fueron iguales pero a > b
	return 0; //Son iguales
}
//METODO PRIVADO: ORGANIZAR MEDIANTE ALG. BURBUJA
//Case es utilizado solo para canbios de Celdas
void HashMap::sort(node* n, string Case = "#", string old = "#") {
	node* head = n;
	int len{1};
	while(head->next != nullptr) {
		len++;
		head = head->next;
	}
	head = n;
	
	for(int i{0}; i < len; ++i) {
		node* sig = head->next;
		for(int j{i + 1}; j < len; ++j) {
			int chg{cmp(head->m_palabra, sig->m_palabra)};
			if(chg > 0 || chg == -2) {
				string word = head->m_palabra;
				string mean = head->m_significado;
				head->m_palabra = sig->m_palabra;
				head->m_significado = sig->m_significado;
				
				sig->m_palabra = word;
				sig->m_significado = mean;
			}
			sig = sig->next;
		}
		head = head->next;
}
	if(Case != "#") {
		node* head = n;
		while(head != nullptr) {
			if(head->m_palabra == Case)
				break;
			
			head = head->next;
		}
		remove(n->m_palabra, old);
		insert(n);
	}
	
}

//METODO PUBLICO: INSERTAR EN MODO LISTA ENLAZADA
bool HashMap::insert(node* n) {
	caseLetter(n->m_palabra);
	int pos{ hash(n) };
	if(pos == -1) return false;
	
	node* head = hashing[pos];
	if(head == nullptr) {
		hashing[pos] = n;
		return true;
	} else {
			while(head->next != nullptr)
				head = head->next;
			
			head->next = n;
			sort(hashing[pos]); //Ordena cada que entra una nueva palabra
			return true;
		}
}

//METODO PUBLICO: MODIFICAR INFORMACION DEL NODO
bool HashMap::modify(string old, string n = "#", string d = "#") {
	caseLetter(old);
	caseLetter(n);
	int pos{ hash(old) };
	node* head = hashing[pos];
	
	while(head != nullptr) {
		if(head->m_palabra == old) {
			break;
		}		
		head = head->next;
	}
	if(n != "#") {
		head->m_palabra = n;
		sort(hashing[pos], n, old);
	}
		
	if(d != "#")
		head->m_significado = d;
	
	return true;
}

//METODO PUBLICO: ELIMINAR UNA PALABRA, SI EXISTE
bool HashMap::remove(string n, string old = "#") {
	caseLetter(n);
	int pos{ hash(n) }; //Letra No Valida (Ex: numero)
	if(old != "#") 
		pos = hash(old);
	
	if(pos == -1) return false;
	
	node* head = hashing[pos]; //Verificar si esta vacio
	if(head == nullptr) return false;
	
	if(head->m_palabra == n) {
		hashing[pos] = head->next;
		head = nullptr;
		return true; //Encontro y asigno la cabezera
	}
	
	while(head->next != nullptr) {
		node* anterior = head;
		head = head->next;
		if(head->m_palabra == n) {
			anterior->next = head->next;
			return true;
		}
	}
	
	return false; //No hubo/encontro coincidencias
}

//METODO PUBLICO: VERIFICA PERTENENCIA EN EL HASHMAP
bool HashMap::belongs(string n) {
	caseLetter(n);
	int pos{ hash(n) };
	if(pos == -1) return false; //No pertenece
	
	node* head = hashing[pos];
	if(head == nullptr) return false; //Esta Vacio
	
	if(head->m_palabra == n) return true; //Es la cabecera
	
	while(head->next != nullptr) {
		head = head->next;
		if(head->m_palabra == n) return true; //Encontro la palabra
	}
	
	return false; //No encontro la palabra, no pertenece
}

//METODO PUBLICO: MUESTRA UNA DEFINICION
void HashMap::def(string n) {
	caseLetter(n);
	int pos{ hash(n) };
	node* head = hashing[pos];
	
	while(head != nullptr) {
		if(head->m_palabra == n)
			break; //Head apunta a la info de la palabra pedida
			
		head = head->next;
	}
	cout << "Palabra: " << head->m_palabra << '\n';
	cout << "Definicion: " << head->m_significado << '\n';	
}

//METODO PUBLICO: MUESTRA INFORMACION DEL HASHMAP
void HashMap::print(string n = "#") {
	n = to_upper(n);
	int pos{ hash(n) };
	bool verify{false}; //true: imprime todo el diccionario
	if(pos == -1) {
		cout << "Error: Letra no existente\n";
		return;
	}
	if(pos == -5) {
		verify = true;
		pos = 0;
	}
	do {
		node* head = hashing[pos];
		cout << "Letra " << orden[pos] << ": \n";
		while(head != nullptr) {
			cout << "-" << head->m_palabra << ": " << head->m_significado << " \n \n";
			head = head->next;
		}
		cout << '\n';
		++pos;
		if(pos == HashSize) verify = false;
		
	} while(verify);
}

//METODO PUBLICO: LEER E INSERTAR NODOS DE UN ARCHIVO
bool HashMap::unPackFile(string file){
	
	ifstream openFile(file);
	
	bool word_meaning{true};
	string file_out = "", word = "", meaning = "";
	while(std::getline(openFile, file_out)) {
	
		if(file_out[0] == '[') {
			for(int i{1}; i < static_cast<int>(file_out.length()) - 1; ++i) {
				if(file_out[i] != ']' && word_meaning) {
					word += file_out[i];
				} else if(file_out[i] == ']') {
						if(belongs(word)) {
							word = "";
							break;
						}	
						word_meaning = false;
						i += 2; //Salto el ". " y empiezo en letra
					} else if(word_meaning == false) {
							meaning += file_out[i];
						}
			}
			
			if(word != "") {
				insert(new node(word, meaning));
				word = meaning = file_out = "";
				word_meaning = true;
			}
			
		} else {
				continue;
			}
	}
	openFile.close();
	
	return true;
}

//METODO PUBLICO:CREAR ARCHIVO->DICCIONARIO ACTUALIZADO
bool HashMap::packingFile() {
	ofstream openFile("Diccionario_Actual.txt");
	for(int i{0}; i < 26; ++i) {
		int pos{ i };
		node* head = hashing[pos];
		if(head == nullptr) continue;
		
		string formatoFile = ""; //Crearemos el formato establecido por si queremos leerlo (al archivo)
		while(head != nullptr) {
			formatoFile += '[';
			formatoFile += head->m_palabra;
			formatoFile += "]. ";
			formatoFile += head->m_significado;
			formatoFile += '.';
			openFile << formatoFile << '\n';
			formatoFile = "";
			head = head->next;
		}
	}
	openFile.close();
	return true;
}

void Error_Handling();

int main(int argc, char *argv[]) {
	HashMap map;
	string word = "", meaning = "", oldWord = "";
	int op{0};
	
	do {
		cout << "Diccionario \n";
		cout << "1. Mostrar diccionario completo \n";
		cout << "2. Mostrar diccionario por letra inicial \n";
		cout << "3. Modificar diccionario \n";
		cout << "4. Buscar definicion \n";
		cout << "5. Leer Archivo \n";
		cout << "6. Crear archivo \n";
		cout << "7. Salir\n";
		cout << "Ingrese opcion: ";
		cin >> op;
		Error_Handling();
		switch(op) {
			case 1:
				system("cls");
				
				map.print();
				
				system("pause");
				system("cls");
			break;
			case 2:
				system("cls");
				
				cout << "Ingrese letra a revisar: ";
				cin >> word;
				system("cls");
				map.print(word);
				
				system("pause");
				system("cls");
			break;
			case 3:
				system("cls");
				
				cout << "1. Ingresar palabra nueva\n";
				cout << "2. Modificar palabra \n";
				cout << "3. Eliminar palabra\n";
				cout << "4. Regresar\n";
				cout << "Ingrese opcion: ";
				cin >> op;
				Error_Handling();
				switch(op) {
					case 1: //Agregar nueva palabra
						system("cls");
						cout << "Ingrese palabra: ";
						std::getline(cin >> std::ws, word);
						
						if(map.belongs(word)) {
							cout << "Palabra ya existente, intente otra diferente\n";
							system("pause");
							system("cls");
							break;
						}
						
						cout << "Ingrese definicion: ";
						std::getline(cin >> std::ws, meaning);
						
						if(map.insert(new node(word, meaning)))
							cout << "Palabra registrada con exito\n";
						else
							cout << "Error: intente nuevamente\n";
						
						system("pause");
						system("cls");
					break;
					case 2: //Modificar palabra
						system("cls");
						cout << "Ingrese palabra a modificar: ";
						std::getline(cin >> std::ws, oldWord);
						if(map.belongs(oldWord) == false) {
							cout << "Palabra no existe en el diccionario\n";
							system("pause");
							system("cls");
							break;
						}
						system("cls");
						cout << "Que desea modificar?\n";
						cout << "1. Palabra\n";
						cout << "2. Significado de la palabra\n";
						cout << "3. Regresar\n";
						cout << "Ingrese opcion: ";
						cin >> op;
						Error_Handling();
						switch(op) {
							case 1:
								system("cls");
								cout << "Ingrese la palabra que modificara a: " << oldWord << '\n';
								std::getline(cin >> std::ws, word);
								if(map.belongs(word)) {
									cout << "Error: La palabra que haz introducido ya existe en el diccionario\n";
									system("pause");
									system("cls");
									break;
								}
								if(map.modify(oldWord, word))
									cout << "Se modifico la palabra correctamente\n";
									
								system("pause");
								system("cls");
							break;
							case 2:
								system("cls");
								cout << "Ingrese nuevo significado para la palabra: " << oldWord << '\n';
								std::getline(cin >> std::ws, meaning);
								if(map.modify(oldWord, "#", meaning))
									cout << "Significado actualizado con exito\n";
									
								system("pause");
								system("cls");
							break;
							case 3:
								system("cls");
							break;
							default:
								cout << "Error: opcion no valida, intente nuevamente\n";
								system("pause");
								system("cls");
						}
					break;
					case 3: //Eliminar palabra
						system("cls");
						
						cout << "Ingresar palabra a eliminar\n";
						std::getline(cin >> std::ws, word);
						
						if(map.belongs(word) == false) {
							cout << "Palabra no existe en el diccionario\n";
							system("pause");
							system("cls");
							break;
						}
						
						if(map.remove(word))
							cout << "Palabra eliminada con exito\n";
						else
							cout << "Error: intente nuevamente\n";
						
						system("pause");
						system("cls");
					break;
					case 4: //Regresar
						system("cls");
					break;
					default:
						cout << "Error: opcion no valida, intente nuevamente\n";
						system("pause");
						system("cls");
				}
			break;
			case 4: //Buscar significado
				system("cls");
				cout << "Ingrese palabra a buscar\n";
				std::getline(cin >> std::ws, word);
				if(map.belongs(word) == false) {
					cout << "Palabra no existe en el diccionario\n";
					system("pause");
					system("cls");
					break;
				}
				system("cls");
				map.def(word);
				
				system("pause");
				system("cls");
			break;
			case 5: //Leer Archivo
				system("cls");
				
				cout << "Ingrese nombre del archivo a procesar: ";
				std::getline(cin >> std::ws, word);
				
				if(map.unPackFile(word))
					cout << "Archivo cargado con exito\n";
				else
					cout << "Archivo no cargado: Problema desconocido\n";
				
				system("pause");
				system("cls");
			break;
			case 6: //Crear Archivo
				system("cls");
				
				if(map.packingFile()) {
					cout << "Archivo: Diccionario_Actual.txt\n";
					cout << "Fue creado con exito\n";
				}
				else
					cout << "Error desconocido\n";
				
				system("pause");
				system("cls");
			break;
			case 7: //Salir
				map.packingFile();
				cout << "Adios :p\n";
				system("pause");
				system("cls");
			break;
			default:
				cout << "Error: opcion no valida, intente nuevamente\n";
				system("pause");
				system("cls");
		}
	} while(op != 7);
	
	return 0;
}

void Error_Handling() {
	if (std::cin.fail())
		{
	    std::cin.clear();
	    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
}