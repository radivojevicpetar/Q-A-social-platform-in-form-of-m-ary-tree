#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>
using namespace std;

enum status {pitanje,komentar,odgovor};
struct Elem {
	string sadrzaj;
	unordered_map<Elem*, int> potomci;
	status tip;
	Elem* predak = nullptr;
	static int pos_id;
	int ID = ++pos_id;
	Elem() {};
};

int Elem::pos_id = 0;
vector<Elem*> pitanja;
Elem* pretraga_pitanja(string s);
Elem* pretraga_odgovora(string p, string s);

void dodaj_pitanje(string s) {
	Elem* el = new Elem();
	el->sadrzaj = s;
	el->tip = pitanje;
	pitanja.push_back(el);
}

void dodaj_odgovor(string p, string s) {
	Elem* el2 = new Elem();
	el2->sadrzaj = s;
	el2->tip = komentar;
	Elem* el = pretraga_pitanja(p);
	auto it = pitanja.begin();
	if (el == nullptr) {
		while (it!=pitanja.end()) {
			el = pretraga_odgovora((*it)->sadrzaj, p);
			if (el != nullptr) break;
			it++;
		}
	}
	else
		el2->tip = odgovor;
	if (el->potomci.size() == 10) {
		cout << "Ne mozete dodati vise odgovora\n";
		return;
	}
	el->potomci.insert(make_pair(el2, 0));
	el2->predak = el;
	}

Elem* pretraga_pitanja(string s) {
	auto it = pitanja.begin();
	while (it != pitanja.end()) {
		if ((*it)->sadrzaj == s) {
			cout << "Pitanje je pronadjeno:" << (*it)->ID << "." << (*it)->sadrzaj << endl;
			return *it;
		}
		it++;
	}
	cout << "Pretraga je neuspesna, pitanje ne postoji\n";
	 return nullptr;
	}


Elem* pretraga_odgovora(string p, string s) {
	Elem* koren = pretraga_pitanja(p);
	if (koren == nullptr)
		return nullptr;
	queue<Elem*> q;
	if (koren->potomci.empty()) {
		cout << "Trazeni odgovor ne postoji\n";
		return nullptr;
	}
	else {
		auto it = koren->potomci.begin();
		while (it!=koren->potomci.end()) {
			q.push((*it).first);
			it++;
		}
	}
	Elem* tmp = nullptr;
	while (!q.empty()) {
		tmp = q.front();
		if (!tmp->potomci.empty()) {
			auto it = tmp->potomci.begin();
			while (it != tmp->potomci.end()) {
				q.push((*it).first);
				it++;
			}
		}
		if (tmp->sadrzaj == s) {
			cout << "Odgovor je pronadjen:" << tmp->ID << "." << tmp->sadrzaj;
			return tmp;
		}
		
		q.pop();
	}
	cout << "Odgovor nije pronadjen\n";
	return nullptr;
}

void inkrementiraj_ocenu(string p,string s,int i) {
	Elem* tmp = pretraga_odgovora(p, s);
	(*tmp->predak->potomci.find(tmp)).second+=i;
}

bool cmp(pair<Elem*, int>& a,pair<Elem*,int>& b)
{
	return a.second > b.second;
}

void sortiraj_odgovore() {
	queue<Elem*> q;
	vector<pair<Elem*, int> > privremeni;
	if (pitanja.empty()) {
		cout << "Ne postoje pitanja\n";
		return;
	}
	auto it = pitanja.begin();
	while (it != pitanja.end()){
		q.push(*it);
		it++;
	}
	Elem* tmp = nullptr;
	while (!q.empty()) {
		tmp = q.front();
		if (!tmp->potomci.empty()) {
			auto it = tmp->potomci.begin();
			while (it != tmp->potomci.end()) {
				q.push((*it).first);
				it++;
			}
			auto it2 = tmp->potomci.begin();
			while (it2 != tmp->potomci.end()) {
				privremeni.push_back(*it2);
				it2++;
			}
			tmp->potomci.clear();
			sort(privremeni.begin(), privremeni.end(), cmp);
			auto it3 = privremeni.begin();
			while (it3 != privremeni.end()) {
				tmp->potomci.insert(make_pair((*it3).first, (*it3).second));
				it3++;
			}
			privremeni.clear();
		}
		q.pop();
		}
		
}

void ispis_svih_pitanja() {
	if (pitanja.empty()) {
		cout << "Ne postoje pitanja\n";
		return;
	}
	auto it = pitanja.begin();
	while (it != pitanja.end()) {
		cout << (*it)->sadrzaj << endl;
		it++;
	}
}

void ispisi_pitanje(string p) {
	Elem* tmp = pretraga_pitanja(p);
	int trenutni_ID = 0;
	queue<Elem*> q;
	if (tmp == nullptr) return;
	cout<<"Pitanje: " << tmp->ID << "." << tmp->sadrzaj << endl;
	trenutni_ID = tmp->ID;
	if (tmp->potomci.empty()) return;
	auto it = tmp->potomci.begin();
	while (it != tmp->potomci.end()) {
		q.push((*it).first);
		it++;
	}
	while (!q.empty()) {
		tmp = q.front();
		q.pop();
		if (tmp->predak->tip == pitanje)
			cout << "Odgovor na pitanje " << tmp->predak->ID<<":  ";
		if (tmp->predak->tip == odgovor)
			cout << "Komentar na odgovor " << tmp->predak->ID << ":  ";
		if (tmp->predak->tip == komentar)
			cout << "Komentar na komentar " << tmp->predak->ID << ":  ";
		cout << tmp->ID << "." << tmp->sadrzaj<<"    ";
		if (!q.empty()&&q.front()->tip==komentar&&q.front()->predak->predak->ID >= trenutni_ID) {
			trenutni_ID = q.front()->predak->ID;
				cout << endl;
		}
		if (q.empty())
			cout << endl;
		if (!tmp->potomci.empty()) {
			auto it = tmp->potomci.begin();
			while (it != tmp->potomci.end()) {
				q.push((*it).first);
				it++;
			}
		}
		
	}
}

void obrisi_odgovor(string p, string s) {
	Elem* tmp=pretraga_odgovora(p, s);
	queue<Elem*> q;
	if (tmp == nullptr) return;
	if (tmp->potomci.empty()) {
		tmp->predak->potomci.erase(tmp->predak->potomci.find(tmp));
		delete tmp;
		return;
	}
	auto it = tmp->potomci.begin();
	while (it != tmp->potomci.end()) {
		q.push((*it).first);
		it++;
	}
	while (!q.empty()) {
		tmp = q.front();
		if (!tmp->potomci.empty()) {
			auto it = tmp->potomci.begin();
			while (it != tmp->potomci.end()) {
				q.push((*it).first);
				it++;
			}
		}
		q.pop();
		tmp->predak->potomci.erase(tmp->predak->potomci.find(tmp));
		delete tmp;
	}
	cout << "Odgovor je obrisan";
}

void obrisi_pitanje(string p){
	Elem* tmp = pretraga_pitanja(p);
	queue<Elem*> q;
	if (tmp == nullptr) return;
	if (tmp->potomci.empty()) delete tmp;
	auto it = tmp->potomci.begin();
	while (it != tmp->potomci.end()) {
		q.push((*it).first);
		it++;
	}
	while (!q.empty()) {
		tmp = q.front();
		if (!tmp->potomci.empty()) {
			auto it = tmp->potomci.begin();
			while (it != tmp->potomci.end()) {
				q.push((*it).first);
				it++;
			}
		}
		q.pop();
		delete tmp;
	}
	cout << "Pitanje je obrisano";
}

Elem* nadji_najbolji_odgovor(string p) {
	Elem* koren = pretraga_pitanja(p);
	int max = 0;
	string max_odg;
	if (koren == nullptr)
		return nullptr;
	queue<Elem*> q;
	if (koren->potomci.empty())
		return nullptr;
	else {
		auto it = koren->potomci.begin();
		while (it != koren->potomci.end()) {
			q.push((*it).first);
			it++;
		}
	}
	Elem* tmp = nullptr;
	while (!q.empty()) {
		tmp = q.front();
		if ((*tmp->predak->potomci.find(tmp)).second > max) {
			(max= (*tmp->predak->potomci.find(tmp)).second);
			max_odg = tmp->sadrzaj;
		}
		if (!tmp->potomci.empty()) {
			auto it = tmp->potomci.begin();
			while (it != tmp->potomci.end()) {
				q.push((*it).first);
				it++;
			}
		}
		q.pop();
	}
	cout << "Najbolji odgovor:";
	return pretraga_odgovora(p, max_odg);
}

int main()
{
	int izbor;
	string s;
	string p;
	int i;

	while (1) {
		cout << endl;
		cout << "Izaberi opciju\n";
		cout << "1.Dodajte novo pitanje\n";
		cout << "2.Dodajte novi odgovor\n";
		cout << "3.Pronadji pitanje\n";
		cout << "4.Pronadji odgovor\n";
		cout << "5.Pronadji najbolji odgovor\n";
		cout << "6.Povecaj ocenu odgovora\n";
		cout << "7.Sortiraj odgovore\n";
		cout << "8.Ispisi sva pitanja\n";
		cout << "9.Ispisi pitanje i njegove odgovore\n";
		cout << "10.Obrisi odgovor i njegove komentare\n";
		cout << "11.Obrisi pitanje i sve njegove odgovore i komentare\n";
		cout << "12.Izadji iz programa\n";
		cin >> izbor;
		cin.ignore();
		switch (izbor) {
		
		case 1: cout << "Unesite pitanje:\n";
			getline(cin, p);
			dodaj_pitanje(p);
			break;
		case 2: cout << "Unesite pitanje i odgovor:\n";
			getline(cin, p);
			getline(cin, s);
			dodaj_odgovor(p, s);
			break;
		case 3: cout << "Unesite pitanje:\n";
			getline(cin, p);
			pretraga_pitanja(p);
			break;
		case 4: cout << "Unesite pitanje i odgovor:\n";
			getline(cin, p);
			getline(cin, s);
			pretraga_odgovora(p, s);
			break;
		case 5: cout << "Unesite pitanje\n";
			getline(cin, p);
			nadji_najbolji_odgovor(p);
			break;
		case 6: cout << "Unesite pitanje, odgovor i inkrement\n";
			getline(cin, p);
			getline(cin, s);
			cin >> i;
			inkrementiraj_ocenu(p, s, i);
			break;
		case 7: sortiraj_odgovore();
			break;
		case 8: ispis_svih_pitanja();
			break;
		case 9: cout << "Unesite pitanje\n";
			getline(cin, p);
			ispisi_pitanje(p);
			break;
		case 10: cout << "Unesite pitanje i odgovor\n";
			getline(cin, p);
			getline(cin, s);
			obrisi_odgovor(p, s);
			break;
		case 11: cout << "Unesite pitanje\n";
			getline(cin, p);
			obrisi_pitanje(p);
			break;
		case 12: exit(0);
		default:break;
		}
	}
	
}