#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include <utility>
#include <iomanip>
#include "DoctorRepo.h"
#include "PatientRepo.h"
#include "ReceptionRepo.h"

using namespace std;

enum option {
	DOC_SPEC_OPT, DOC_CAB_OPT, DOC_PLOT_OPT, DOC_TT_OPT,

	PAT_ADDRESS_OPT
};

enum print_option {
	DOC_ALL, DOC_INTO,

	PAT_ALL, PAT_INTO,

	REC_ALL, REC_INTO
};

enum su_option {
	DOC_SELECT, DOC_UPDATE, DOC_INSERT,

	PAT_SELECT, PAT_UPDATE, PAT_INSERT,

	REC_INSERT, REC_UPDATE
};

template<typename T>
T* getVectorItem(const vector<T*>& vector, int num) {
	for (int i = 0; i < vector.size(); i++) {
		if ((i + 1) == num) {
			return vector.at(i);
		}
	}
}

vector<Doctor*> sortDoctorsTimetable(vector<Doctor*> doctors) {
	for (int i = 0; i < doctors.size(); i++) {
		doctors.at(i)->sortTimetable();
	}

	return doctors;
}

string checkDiagnosisFormat(su_option opt) {
	string str;

	//cin.ignore((numeric_limits<streamsize>::max)(), '\n');
	while (true) {
		getline(cin, str);

		if (opt == REC_UPDATE) {
			if (str == "0")
				return str;
		}

		if (str.length() > 5) break;
		else {
			cout << "Incorrect diagnosis (more 5 simbols)!" << endl;
			str = "";
		}
	}

	return str;
}

map<string, string> checkAnalysesFormat() {
	map<string, string> analyses;

	string str = "";

	getline(cin, str);

	if (str == "0")
		return analyses;

	// Алгоритм разделения строк по пробелу
	istringstream ist(str);
	while (ist >> str) {
		analyses.insert(pair<string, string>(str.c_str(),"Not Passed"));
	}

	return analyses;
}

vector<string> checkProceduresFormat() {
	vector<string> procedures;
	string str = "";
	vector<string> strap;

	//cin.ignore((numeric_limits<streamsize>::max)(), '\n');
	while (true) {
		getline(cin, str);

		if (str == "0")
			break;

		strap.push_back(str);
	}

	for (int i = 0; i < strap.size(); i++) {
		procedures.push_back(strap.at(i).c_str());
	}

	return procedures;
}

vector<string> checkMedicationsFormat() {
	vector<string> medications;
	string str = "";

	cin.ignore((numeric_limits<streamsize>::max)(), '\n');
	getline(cin, str);

	if (str == "0")
		return medications;

	// Алгоритм разделения строк по пробелу
	istringstream ist(str);
	while (ist >> str) {
		medications.push_back(str.c_str());
	}

	return medications;
}

string checkComplaintsFormat(su_option opt) {
	string str;

	//cin.ignore((numeric_limits<streamsize>::max)(), '\n');
	while (true) {
		getline(cin, str);

		if (opt == REC_UPDATE) {
			if (str == "0")
				return str;
		}

		if (str.length() > 10) break;
		else {
			cout << "Incorrect complaints (more 10 simbols)!" << endl;
			str = "";
		}
	}

	return str;
}

string checkAddressFormat(su_option opt) {
	// 4 строки через пробел (или через enter)
	string str;

	string city;
	string street;
	string building;
	string apartment;

	while (true) {
		cout << "City: ";
		cin >> city;
		cout << "Street: ";
		cin >> street;
		cout << "Building: ";
		cin >> building;
		cout << "Apartment: ";
		cin >> apartment;
		cout << endl;

		if (opt == PAT_UPDATE) {
			if (city == "0" || street == "0" || building == "0" || apartment == "0") {
				str = "0";
				return str;
			}
		}

		// Если в полях apartment и building есть цифры, то ок, если нет, то неверно
		if (std::any_of(building.cbegin(), building.cend(), ::isdigit) &&
			std::any_of(apartment.cbegin(), apartment.cend(), ::isdigit)) break;
		else {
			cout << "Incorrect address (building or apartment format)!" << endl;
			str = "";
		}
	}

	str = city + ',' + street + ',' + building + ',' + apartment;

	return str;
}

void printAnalyses(vector<string> analyses) {
	for (int i = 0; i < analyses.size(); i++) {
		cout << "        " << i + 1 << ") " << analyses.at(i) << endl;
	}
}

string choiseAnalysis(vector<string> analyses, string message) {
	int tmp;
	string str;
	printAnalyses(analyses);
	cout << message << endl;

	while (true) {
		cin >> tmp;

		if (tmp == 0) {
			return "0";
		}
		if (tmp < 1 || tmp > analyses.size()) {
			cout << "Incorrect number!" << endl;
			continue;
		}
		else {
			str = analyses.at(tmp - 1);
			return str;
		}
	}

	return "0";
}

void printReceptions(const vector<Reception*>& receptions, print_option opt) {
	if (opt == REC_ALL) {
		cout << "Output format: \n" << 
			"        (Doctor - Specialization - Plot - Cabinet\n"
			"        (Patient - City,Street,Building,Apartment)\n"
			"        (Date Of Reception - Date Of Extract)\n"
			"        (Time Of Reception)\n"
			"        (Closed or not)" << endl << endl;
		cout << "All receptions: " << endl;
	}

	std::stringstream ss;
	string closed = "";
	string dateOfExtract = "";
	for (int i = 0; i < receptions.size(); i++) {
		ss << i + 1;
		closed = receptions.at(i)->getIsDone() == 1 ? "Closed" : "Not Closed";
		dateOfExtract = receptions.at(i)->getDateOfExtract() == "" ? "Not Detected" : receptions.at(i)->getDateOfExtract();

		cout << "     " << i + 1 << ") " << receptions.at(i)->getDoctor().getName() << " - "
			<< receptions.at(i)->getDoctor().getSpecialization() << " - "
			<< receptions.at(i)->getDoctor().getPlot() << " - "
			<< receptions.at(i)->getDoctor().getCabinet() << endl

			<< "        " << setw(ss.str().length())
			<< receptions.at(i)->getPatient().getName() << " - "
			<< receptions.at(i)->getPatient().getAddress() << endl

			<< "        " << setw(ss.str().length())
			<< receptions.at(i)->getDateOfReception() << " - "
			<< dateOfExtract << endl

			<< "        " << setw(ss.str().length())
			<< receptions.at(i)->getTimeOfReception() << endl

			<< "        " << setw(ss.str().length())
			<< closed << endl;

		ss.clear();
	}
}

void printReception(const Reception* reception, print_option opt) {
	if (opt == REC_INTO) {
		cout << "Назначенные лекарства: " << endl;
		if (reception->getMedications().empty()) {
			cout << "No medications" << endl;
			return;
		}
		else
			for (int i = 0; i < reception->getMedications().size(); i++) {
				cout << "\t" << i + 1 << ") " << reception->getMedications().at(i) << endl;
			}

		return;
	}

	if (opt == REC_ALL) {
		cout << "Output format: \n" <<
			"        (Doctor - Specialization - Plot - Cabinet\n"
			"        (Patient - City,Street,Building,Apartment)\n"
			"        (Date Of Reception - Date Of Extract)\n"
			"        (Time Of Reception)\n"
			"        (Closed or not)\n"
			"        (Complaints)\n"
			"        (Diagnosis)\n"
			"        (Medications)\n"
			"        (Procedures)\n"
			"        (Analysis - Analysis Result)" << endl << endl;
		cout << "Reception: " << endl;
	}

	string closed = reception->getIsDone() == 1 ? "Closed" : "Not Closed";
	string dateOfExtract = reception->getDateOfExtract() == "" ? "Not Detected" : reception->getDateOfExtract();
	string complaints = reception->getComplaints() == "" ? "No Complaints" : reception->getComplaints();
	string diagnosis = reception->getDiagnosis() == "" ? "No Diagnosis" : reception->getDiagnosis();

	cout << reception->getDoctor().getName() << " - "
		<< reception->getDoctor().getSpecialization() << " - "
		<< reception->getDoctor().getPlot() << " - "
		<< reception->getDoctor().getCabinet() << endl
		<< reception->getPatient().getName() << " - "
		<< reception->getPatient().getAddress() << endl
		<< reception->getDateOfReception() << " - "
		<< dateOfExtract << endl
		<< reception->getTimeOfReception() << endl
		<< closed << endl

		<< "Жалобы: " << endl
		<< "\t- " << complaints << endl

		<< "Диагноз: " << endl
		<< "\t- " << diagnosis << endl

		<< "Назначенные лекарства: " << endl;
		if (reception->getMedications().empty()) {
			cout << "\t- " << "No Medications" << endl;
		}
		else
			for (int i = 0; i < reception->getMedications().size(); i++) {
				cout << "\t- " << reception->getMedications().at(i) << endl;
			}
		
		cout << "Назначенные процедуры: " << endl;
		if (reception->getProcedures().empty()) {
			cout << "\t- " << "No Procedures" << endl;
		}
		else
			for (int i = 0; i < reception->getProcedures().size(); i++) {
				cout << "\t- " << reception->getProcedures().at(i) << endl;
			}

		cout << "Анализы: " << endl
		<< "\t " << "Анализ - Результат анализа: " << endl;
		if (reception->getAnalyses().empty()) {
			cout << "\t- " << "No Analyses" << endl;
		}
		else
			for (auto& item : reception->getAnalyses()) {
				if(!item.second.empty())
					cout << "\t- " << item.first << " - " << item.second << endl;
				else
					cout << "\t- " << item.first << " - " << "Analysis Not Submitted" << endl;
			}
}

void printReceptionAnalysis(Reception* reception) {
	int i = 0;
	for (auto& item : reception->getAnalyses()) {
		cout << "\t" << i + 1 << ") " << item.first << endl;
		i++;
	}
}

int choiseAnalysis(Reception* reception, string message) {
	int tmp;
	printReceptionAnalysis(reception);
	cout << message << endl;

	while (true) {
		cin >> tmp;

		if (tmp == 0) {
			return 0;
		}
		if (tmp < 1 || tmp > reception->getAnalyses().size()) {
			cout << "Incorrect number!" << endl;
			continue;
		}
		else return tmp;
	}

	return 0;
}

int choiseMedication(Reception* reception, string message, print_option opt) {
	int tmp;
	printReception(reception, opt);
	cout << message << endl;

	while (true) {
		cin >> tmp;

		if (tmp == 0) {
			return 0;
		}
		if (tmp < 1 || tmp > reception->getMedications().size()) {
			cout << "Incorrect number!" << endl;
			continue;
		}
		else return tmp;
	}

	return 0;
}

void printPatients(const vector<Patient*>& patients, print_option opt) {
	if (opt == PAT_ALL) {
		cout << "Output format: " << "(Patient - City,Street,Building,Apartment)" << endl;
		cout << "All patients: " << endl;
	}
	for (int i = 0; i < patients.size(); i++) {
		cout << "     " << i + 1 << ") " << patients[i]->getName() << " - "
			<< patients[i]->getAddress() << endl;
	}
}

void printDoctorTimetable(const Doctor& doctor, DoctorRepo doctorRepo, print_option opt) {
	vector<pair<string, pair<string, string>>> splitTime;
	splitTime = doctorRepo.splitDoctorTimetable(doctor);

	if (opt == DOC_ALL) {
		cout << "Output format: \n" << "        (Doctor - Specialization\n"
		    "        (Monday:    time begin - time end)\n"
			"        (Tuesday:   time begin - time end)\n"
			"        (Wednesday: time begin - time end)\n"
			"        (Thursday:  time begin - time end)\n"
			"        (Friday:    time begin - time end)\n"
			"        (Saturday:  time begin - time end)\n"
			"        (Sunday:    weekend)" << endl << endl;

		cout << "     " << doctor.getName() << " - " << doctor.getSpecialization() << endl;
		for (int i = 0; i < splitTime.size(); i++) {
			cout << "     " << splitTime.at(i).first << ": " << splitTime.at(i).second.first <<
				" - " << splitTime.at(i).second.second << endl;
		}
	}
	else if (opt == DOC_INTO) {
		cout << doctor.getName() << " - " << doctor.getSpecialization() << endl;
		for (int i = 0; i < splitTime.size(); i++) {
			cout << "     " << i + 1 << ") " << splitTime.at(i).first << ": " << splitTime.at(i).second.first <<
				" - " << splitTime.at(i).second.second << endl;
		}
	}
}

void printDoctors(const vector<Doctor*>& doctors, print_option opt) {
	if (opt == DOC_ALL) {
		cout << "Output format: " << "(Doctor - Specialization - Cabinet - Plot)" << endl;
		cout << "All doctors: " << endl;
	}
	for (int i = 0; i < doctors.size(); i++) {
		cout << "     " << i + 1 << ") " << doctors[i]->getName() << " - "
			<< doctors[i]->getSpecialization() << " - "
			<< doctors[i]->getCabinet() << " - "
			<< doctors[i]->getPlot() << endl;
	}
}

string checkNameFormat(su_option opt) {
	// 2 или 3 слова через пробел
	string str;
	cin.ignore((numeric_limits<streamsize>::max)(), '\n');
	int tmpHelp = 0;
	while (true) {
		getline(cin, str);

		if (opt == DOC_UPDATE || opt == PAT_UPDATE) {
			if (str == "0")
				return str;
		}

		for (int i = 0; i < str.length(); i++) {
			if (str[i] == ' ') tmpHelp++;
		}

		if (tmpHelp == 2 || tmpHelp == 1) break;
		else {
			cout << "Incorrect full name (format)!" << endl;
			str = "";
			tmpHelp = 0;
		}
	}

	return str;
}

int converStringToInteger(string str) {
	string min = "";
	string sec = "";

	for (int i = 0; i < str.length(); i++) {
		if (str[i] == ':') {
			min = str.substr(0,i);
			sec = str.substr(i + 1, 2);
			break;
		}
	}

	string res = min + sec;
	return atoi(res.c_str());
}

vector <pair<string, string>> choise(DoctorRepo doctorRepo, option opt, su_option opt2) {
	int d_num;
	int d_num2;
	int d_num3;
	string tmpStr;
	int tmpNumber;
	vector <pair<string, string>> data;

	switch(opt) {
	case DOC_SPEC_OPT:
				tmpStr = doctorRepo.getAllSpecializations();
				tmpNumber = doctorRepo.getCountSpecializations();

				if (opt2 == DOC_SELECT) {
					cout << "Choose doctor's specialization to insert: " << endl
						<< tmpStr << endl;
				}
				else if (opt2 == DOC_UPDATE) {
					cout << "Choose doctor's specialization to update (0 to skip this parametr): " << endl
						<< tmpStr << endl;
				}
				break;
	case DOC_CAB_OPT:
				tmpStr = doctorRepo.getAllCabinets();
				tmpNumber = doctorRepo.getCountCabinets();

				if(opt2 == DOC_SELECT) {
					cout << "Choose doctor's cabinet to insert: " << endl
						<< tmpStr << endl;
				}
				else if (opt2 == DOC_UPDATE) {
					cout << "Choose doctor's cabinet to update (0 to skip this parametr): " << endl
						<< tmpStr << endl;
				}
				break;
	case DOC_PLOT_OPT:
				tmpStr = "";
				tmpNumber = doctorRepo.getCountPlots();

				for (int i = 0; i < tmpNumber; i++) {
					tmpStr += std::to_string(i + 1) + ")" + " " + std::to_string(i + 1) + "\n";
				}

				if(opt2 == DOC_SELECT) {
					cout << "Choose doctor's plot to insert: " << endl
						<< tmpStr << endl;
				}
				else if (opt2 == DOC_UPDATE) {
					cout << "Choose doctor's plot to update (0 to skip this parametr): " << endl
						<< tmpStr << endl;
				}
				break;
	case DOC_TT_OPT:
		tmpStr = doctorRepo.getAllDaysOfWeek();
		tmpNumber = 7;
		string delimiter = " ";

		string tmpStr2 = "";
		int tmpNumber2 = 0;
		string tmpStr3 = "";
		int tmpNumber3 = 0;

		while (true) {
			if (opt2 == DOC_SELECT) {
				cout << "Choose day of week to insert (0 to skip this parametr): " << endl
					<< tmpStr << endl;
			}
			else if (opt2 == DOC_UPDATE) {
				cout << "Choose day of week to update (0 to skip this parametr): " << endl
					<< tmpStr << endl;
			}

			cin >> d_num;
			if (d_num == 0) {
				if(opt2 == DOC_SELECT)
					break;
				else if (opt2 == DOC_UPDATE) {
					return data;
				}
			}
			else
				if (d_num < 1 || d_num > tmpNumber && d_num != 0) {
					cout << "Incorrect number!" << endl;
					continue;
				}
			else {
				if(tmpStr2.length() == NULL) tmpStr2 = doctorRepo.getAllTimesOfJob();
				if (tmpNumber2 == 0) tmpNumber2 = doctorRepo.getCountTimesOfJob();

				cout << "Choose time interval to job (0 to return back): " << endl
					<< tmpStr2 << endl;

				while (true) {
					cin >> d_num2;
					cin >> d_num3;
					if (d_num2 == 0 || d_num3 == 0) break;
					else 
						if ((d_num2 < 1 || d_num2 > tmpNumber2) || (d_num3 < 1 || d_num3 > tmpNumber2)) {
								cout << "Incorrect number!" << endl;
								continue;
							}
						else {
								string timeBegin;
								// Взять время (начала)
								tmpNumber3 = tmpStr2.find(std::to_string(d_num2) + ')');
								string tmpStrHelp = tmpStr2.substr(tmpNumber3);
								tmpStrHelp = tmpStrHelp.substr(tmpStrHelp.find(')') + 2);
								for (int i = 0; i < tmpStrHelp.length(); i++) {
									if (tmpStrHelp[i] == '\n') {
										timeBegin = tmpStrHelp.substr(0, i);
										break;
									}
								}

								string timeEnd;
								// Взять время (конца)
								tmpNumber3 = tmpStr2.find(std::to_string(d_num3) + ')');
								tmpStrHelp = tmpStr2.substr(tmpNumber3);
								tmpStrHelp = tmpStrHelp.substr(tmpStrHelp.find(')') + 2);
								for (int i = 0; i < tmpStrHelp.length(); i++) {
									if (tmpStrHelp[i] == '\n') {
										timeEnd = tmpStrHelp.substr(0, i);
										break;
									}
								}

								if (converStringToInteger(timeBegin) < converStringToInteger(timeEnd)) {
									string tmpNew;
									// Получение выбранного дня недели
									int tmpNumberHelp = tmpStr.find(std::to_string(d_num) + ')');
									string tmpStr2 = tmpStr.substr(tmpNumberHelp);
									tmpStr2 = tmpStr2.substr(tmpStr2.find(')') + 2);
									for (int i = 0; i < tmpStr2.length(); i++) {
										if (tmpStr2[i] == '\n') {
											tmpNew = tmpStr2.substr(0, i);
											break;
										}
									}

									if (data.empty()) {
										data.push_back(make_pair(tmpNew, timeBegin + delimiter + timeEnd));
									}
									else {
										int it = 0;
										while(it < data.size()) {
											if (data.at(it).first == tmpNew) {
												data.at(it).second = timeBegin + delimiter + timeEnd;
												break;
											}

											it++;
										}
										if(it >= data.size()) 
											data.push_back(make_pair(tmpNew, timeBegin + delimiter + timeEnd));
									}
								}
								else {
									cout << "Incorrect choising time(negative interval)!" << endl;
									continue;
								}
							}
					break;
				}
			}
		}

		return data;
		break;
	}

	if (opt2 == DOC_SELECT) {
		while (true) {
			cin >> d_num;
			if (d_num < 1 || d_num > tmpNumber) {
				cout << "Incorrect number!" << endl;
				continue;
			}
			else break;
		}
	}
	else if (opt2 == DOC_UPDATE) {
		while (true) {
			cin >> d_num;
			if (d_num == 0) {
				data.clear();
				return data;
			}
			else if (d_num < 1 || d_num > tmpNumber) {
				cout << "Incorrect number!" << endl;
				continue;
				}
				else break;
		}
	}
	

	tmpNumber = tmpStr.find(std::to_string(d_num) + ')');
	string tmpStr2 = tmpStr.substr(tmpNumber);
	tmpStr2 = tmpStr2.substr(tmpStr2.find(')') + 2);
	for (int i = 0; i < tmpStr2.length(); i++) {
		if (tmpStr2[i] == '\n') {
			tmpStr = tmpStr2.substr(0, i);
			break;
		}
	}

	data.push_back(make_pair(tmpStr, std::to_string(tmpNumber)));
	return data;
}

int choiseIntoListReceptions(const vector<Reception*>& receptions, string message, print_option opt) {
	int tmp;
	printReceptions(receptions, opt);
	cout << message << endl;

	while (true) {
		cin >> tmp;

		if (tmp == 0) {
			return 0;
		}
		if (tmp < 1 || tmp > receptions.size()) {
			cout << "Incorrect number!" << endl;
			continue;
		}
		else return tmp;
	}

	return 0;
}

int choiseIntoListPatient(const vector<Patient*>& pats, string message) {
	int tmp;
	cout << message << endl;
	printPatients(pats, PAT_INTO);

	while (true) {
		cin >> tmp;

		if (tmp == 0) {
			return 0;
		}
		if (tmp < 1 || tmp > pats.size()) {
			cout << "Incorrect number!" << endl;
			continue;
		}
		else return tmp;
	}

	return 0;
}

int choiseIntoListDoctor(const vector<Doctor*>& docs, string message) {
	int tmp;
	cout << message << endl;
	printDoctors(docs, DOC_INTO);

	while (true) {
		cin >> tmp;

		if (tmp == 0) {
			return 0;
		}
		if (tmp < 1 || tmp > docs.size()) {
			cout << "Incorrect number!" << endl;
			continue;
		}
		else return tmp;
	}

	return 0;
}

vector<int> choiseIntoListTimetableDoctor(Doctor* doc, DoctorRepo doctorRepo, string message, print_option opt) {
	int tmp = 0;
	vector<int> d_num_list;

	cout << message << endl;
	
	if (opt == DOC_INTO) {
		printDoctorTimetable(doc, doctorRepo, opt);
		while (true) {
			cin >> tmp;

			if (tmp == 0) break;
			if (tmp < 1 || tmp > doc->getTimetable().size()) {
				cout << "Incorrect number!" << endl;
				continue;
			}
			else {
				int inter = 0;
				for (int i = 0; i < d_num_list.size(); i++) {
					if (d_num_list.at(i) == tmp) {
						inter++;
						break;
					}
				}
				if (inter == 0) d_num_list.push_back(tmp);
			}
		}
	}
	else if (opt == DOC_ALL) {
		printDoctorTimetable(doc, doctorRepo, DOC_INTO);
		while (true) {
			cin >> tmp;

			if (tmp == 0) break;
			if (tmp < 1 || tmp > doc->getTimetable().size()) {
				cout << "Incorrect number!" << endl;
				continue;
			}
			else {
				d_num_list.push_back(tmp);
				break;
			}
		}
	}
	

	if(tmp == 0) d_num_list.push_back(0);

	return d_num_list;
}

string checkDateFormat(su_option opt) {
	string str;
	list<int> words;
	string tmp;
	int ex = 0;
	int currYear;
	int cinYear;
	int cinMonth;
	int cinDay;

	const time_t tm = time(nullptr);
	char buf[64];
	strftime(buf, std::size(buf), "%Y-%m-%d", localtime(&tm));
	string currentDate = string(buf);

	cout << "Insert wish date to next format YYYY-MM-DD (0 to exit): " << endl;
	//cin.ignore((numeric_limits<streamsize>::max)(), '\n');
		
	while (true) {
		cin >> str;

		if (opt == REC_INSERT) {
			if (str == "0") {
				str = "0";
				return str;
			}
		}

		if (str == "" || str.empty()) {
			cout << "Incorrect date (empty string)!" << endl;
			continue;
		}

		// Замена "-" на " "
		for (int i = 0; i < str.length(); i++) {
			if ((i + 1) < str.length())
				if (str[i] == '-' && str[i + 1] == '-') continue;

			if (str[i] == '-') {
				str[i] = ' ';
			}
		}

		// Алгоритм разделения строк по пробелу
		istringstream ist(str);
		while (ist >> tmp) {
			if (all_of(tmp.begin(), tmp.end(), ::isdigit)) { // Если вся строка это число
				words.push_back(atoi(tmp.c_str()));
			}
			else break;
		}

		if (words.size() > 3 || words.size() < 3) {
			cout << "Incorrect date (date format)!" << endl;
			str = "";
			words.clear();
			continue;
		}

		ex = 0;
		list<int>::iterator it = words.begin();
		currYear = atoi(currentDate.substr(0, 4).c_str());
		cinYear = *next(it, 0);
		cinMonth = *next(it, 1);
		cinDay = *next(it, 2);
		// Если цифры в корректном диапазоне
		if ((cinYear <= (currYear + 1) && (cinYear > (currYear - 1)))
			&&
			(cinMonth <= 12 && cinMonth > 0)
			&&
			(cinDay <= 31 && cinDay > 0)) {
			
			switch (cinMonth) {
			case 1:
				if (cinDay <= 31 && cinDay > 0) ex++;
				break;
			case 2:
				if(cinYear % 4 == 0) { // Если год високосный
					if (cinDay <= 29 && cinDay > 0) ex++;
				}
				else { // Если год не високосный
					if (cinDay <= 28 && cinDay > 0) ex++;
				}
				break;
			case 3:
				if (cinDay <= 31 && cinDay > 0) ex++;
				break;
			case 4:
				if (cinDay <= 30 && cinDay > 0) ex++;
				break;
			case 5:
				if (cinDay <= 31 && cinDay > 0) ex++;
				break;
			case 6:
				if (cinDay <= 30 && cinDay > 0) ex++;
				break;
			case 7:
				if (cinDay <= 31 && cinDay > 0) ex++;
				break;
			case 8:
				if (cinDay <= 31 && cinDay > 0) ex++;
				break;
			case 9:
				if (cinDay <= 30 && cinDay > 0) ex++;
				break;
			case 10:
				if (cinDay <= 31 && cinDay > 0) ex++;
				break;
			case 11:
				if (cinDay <= 30 && cinDay > 0) ex++;
				break;
			case 12:
				if (cinDay <= 31 && cinDay > 0) ex++;
				break;
			}
		}
		else {
			cout << "Incorrect date (date out of bounce (year, month or day))!" << endl;
			str = "";
			words.clear();
			continue;
		}

		// Если дата введена верно
		if (ex > 0) {
			// Замена " " на "-"
			for (int i = 0; i < str.length(); i++) {
				if (str[i] == ' ') {
					str[i] = '-';
				}
			}

			return str;
		}
		else {
			cout << "Incorrect date (date out of bounce (day in the wrong month))!" << endl;
			str = "";
			words.clear();
		}
	}

	return str;
}

string checkTimeReception(vector<pair<string,pair<string, string>>> time) {
	string str;
	string beginFirst = time.at(0).second.first.substr(0, 2);
	string endFirst = time.at(0).second.first.substr(3,2);
	string beginSecond = time.at(0).second.second.substr(0,2);
	string endSecond = time.at(0).second.second.substr(3,2);
	list<int> words;
	string tmp;

	cout << "Insert wish time at '" << time.at(0).first << "' to next format HH:MM (0 to exit): " << endl;
	cout << "    " << time.at(0).first << ": " << beginFirst << ":" << endFirst << " - "
		<< beginSecond << ":" << endSecond << endl;
	//cin.ignore((numeric_limits<streamsize>::max)(), '\n');

	while (true) {
		cin >> str;

		if (str == "0") return str;
		
		// Замена ":" на " "
		for (int i = 0; i < str.length(); i++) {
			if (str[i] == ':') {
				str[i] = ' ';
			}
		}

		// Алгоритм разделения строк по пробелу
		istringstream ist(str);
		while (ist >> tmp) {
			if (all_of(tmp.begin(), tmp.end(), ::isdigit)) { // Если вся строка это число
				words.push_back(atoi(tmp.c_str()));
			}
			else break;
		}

		if (words.size() > 2 || words.size() < 2) {
			cout << "Incorrect time (time format)!" << endl;
			str = "";
			words.clear();
			continue;
		}

		list<int>::iterator it = words.begin();
		// Если точно в диапазоне
		if (*next(it, 0) > atoi(beginFirst.c_str()) && *next(it, 0) < atoi(beginSecond.c_str())) {
			// Если не позже, чем за 15 минут до конца рабочего дня
			if (*next(it, 0) + 1 < atoi(beginSecond.c_str())) {
				if (*next(it, 1) >= 0 && *next(it, 1) < 60) {

					for (int i = 0; i < str.length(); i++) {
						if (str[i] == ' ') {
							str[i] = ':';
						}
					}
					return str;
				}
			}
			else if (*next(it, 0) + 1 == atoi(beginSecond.c_str())) {
				// Если граница это 0
				if (atoi(endSecond.c_str()) == 0) {
					if (*next(it, 1) <= 45) {

						for (int i = 0; i < str.length(); i++) {
							if (str[i] == ' ') {
								str[i] = ':';
							}
						}
						return str;
					}
					else {
						cout << "Incorrect time (time out of bounds of doctor's work time)!" << endl;
						str = "";
						words.clear();
						continue;
					}
				}
				else {
					if (*next(it, 1) <= (atoi(endSecond.c_str()) - 15)) {

						for (int i = 0; i < str.length(); i++) {
							if (str[i] == ' ') {
								str[i] = ':';
							}
						}
						return str;
					}
					else {
						cout << "Incorrect time (time out of bounds of doctor's work time)!" << endl;
						str = "";
						words.clear();
						continue;
					}
				}
				
			}
			
		} // Если на границах диапазона
		else if (*next(it, 0) == atoi(beginFirst.c_str()) || *next(it, 0) == atoi(beginSecond.c_str())) {
			if (*next(it, 0) == atoi(beginFirst.c_str())) {
				if (*next(it, 1) >= atoi(endFirst.c_str())) {

					for (int i = 0; i < str.length(); i++) {
						if (str[i] == ' ') {
							str[i] = ':';
						}
					}
					return str;
				}
			}
			else if (*next(it, 0) == atoi(beginSecond.c_str())) {
				if (atoi(endSecond.c_str()) > 0) {
					if (*next(it, 1) <= (atoi(endSecond.c_str()) - 15)) {

						for (int i = 0; i < str.length(); i++) {
							if (str[i] == ' ') {
								str[i] = ':';
							}
						}
						return str;
					}
				} // Если прямо на конечной границе
				else if (atoi(endSecond.c_str()) == 0) {
					if (*next(it, 1) == atoi(endSecond.c_str())) {
						cout << "Incorrect time (time out of bounds of doctor's work time)!" << endl;
						str = "";
						words.clear();
						continue;
					}
				}
			}
		}
		else {
			cout << "Incorrect time (time out of bounds of doctor's work time)!" << endl;
			str = "";
			words.clear();
			continue;
		}
	}

	return str;
}

void loop() {
	DB_Operations db = DB_Operations();

	DoctorRepo doctorRepo(db);
	vector<Doctor*> doctors;
	Doctor doctor;

	PatientRepo patientRepo(db);
	vector<Patient*> patients;
	Patient patient;

	ReceptionRepo receptionRepo(db);
	vector<Reception*> receptions;
	Reception reception;

	string str;
	string str2;
	int d_num;
	int d_num2;
	vector<int> d_num_list;
	int maxId = 0;
	vector <pair<string, string>> data;
	pair<string, pair<string, string>> timetablePosition;

	vector<string> medications;
	vector<string> procedures;
	map<string,string> analyses;

	int operation;

	while (true) {
		cout << "0. Exit" << endl;
		cout << "-----------------------------------------------------" << endl;
		cout << "Doctors" << endl;
		cout << "1. List all doctors" << endl;
		cout << "2. Add a doctor" << endl;
		cout << "3. Edit a doctor" << endl;
		cout << "4. Delete doctor" << endl;
		cout << "5. Edit doctor's timetable" << endl; // edit + add
		cout << "6. Delete doctor's timetable position" << endl;
		cout << "7. Show doctor's timetable" << endl;
		cout << "-----------------------------------------------------" << endl;
		cout << "Patients" << endl;
		cout << "8. List all patients" << endl;
		cout << "9. Add a patient" << endl;
		cout << "10. Edit a patient" << endl;
		cout << "11. Delete a patient" << endl;
		cout << "-----------------------------------------------------" << endl;
		cout << "Receptions" << endl;
		cout << "12. List all receptions" << endl; // Список всех "приёмов" с возможностью "раскрыть" запись и более подробно ознакомиться (findAll)
		cout << "13. Registration for a reception" << endl; // Запись пациента на приём (add)
		cout << "14. Add treatment at reception" << endl; // Добавление лечения
		//cout << "15. Edit treatment at reception" << endl; // Редактирование лечения
		cout << "15. Edit a reception" << endl; // Внесение изменений приёма (жалобы + диагноз + смена врача (не закрытие)) (edit)
		cout << "16. Closing reception" << endl; // Закрыть приём ("выписка" пациента) (edit)
		cout << "17. Delete a reception" << endl; // Удаление приёма из базы данных (remove)
		/*cout << "-----------------------------------------------------" << endl;
		cout << "Search" << endl;
		cout << "Search_Doctors" << endl;
		cout << "19. Find doctor by full name" << endl;
		cout << "20. Find doctors by plot" << endl;
		cout << "21. Find doctors by patient" << endl;
		cout << "22. Find doctors by cabinet" << endl;
		cout << "23. Find doctors by specialization" << endl;
		cout << "24. Find doctors by time of reception (day of week)" << endl;
		cout << "25. Find doctors by time of reception (day of week + time interval)" << endl;
		cout << "Search_Doctors_Timetable" << endl;
		cout << "26. Find timetable of doctor (by name + by plot + by cabinet + by specialization)" << endl;
		cout << "Search_Patients" << endl;
		cout << "27. Find patient by full name" << endl;
		cout << "28. Find patient by address" << endl;
		cout << "29. Find patients by procedures (select from list)" << endl;
		cout << "30. Find patients by analyses results (select from list)" << endl;
		cout << "Search_Receptions" << endl;
		cout << "31. Find receptions by doctor (by name + by plot + by cabinet + by specialization)" << endl;
		cout << "32. Find receptions by patient (by name + by address)" << endl;*/
		cout << "-----------------------------------------------------" << endl;

		cin >> operation;

		switch (operation) {
		case 0:
			return;
		case 1: {
			if (doctorRepo.getMaxDoctorId() == 0) {
				cout << "List doctors is empty!" << endl << endl;
				break;
			}
			printDoctors(doctorRepo.findAll(), DOC_ALL);
			doctors = doctorRepo.findAll();
			doctors = sortDoctorsTimetable(doctors);
			break;
			}
		case 2: {
			cout << "Enter doctor's full name: " << endl;
			str = checkNameFormat(DOC_INSERT);
			maxId = doctorRepo.getMaxDoctorId();
			if (maxId != 0) {
				doctor.setId(maxId + 1);
			}
			else {
				doctorRepo.alterSeqDoctors(1);
				doctor.setId(1);
			}
			doctor.setName(str);
			doctor.setSpecialization(choise(doctorRepo, DOC_SPEC_OPT, DOC_SELECT).at(0).first); // Взять из pair именно string
			doctor.setCabinet(choise(doctorRepo, DOC_CAB_OPT, DOC_SELECT).at(0).first);
			doctor.setPlot(atoi(choise(doctorRepo, DOC_PLOT_OPT, DOC_SELECT).at(0).first.c_str()));
			doctor.setTimetable(choise(doctorRepo, DOC_TT_OPT, DOC_SELECT));

			doctorRepo.add(doctor);

			doctors = doctorRepo.findAll();
			doctors = sortDoctorsTimetable(doctors);
			break;
			}
		case 3: {
			if (doctorRepo.getMaxDoctorId() == 0) {
				cout << "List doctors is empty!" << endl << endl;
				break;
			}

			doctors = doctorRepo.findAll();
			d_num = choiseIntoListDoctor(doctors, "Choise doctor into list to update (0 to exit): ");

			if (d_num > 0) {
				Doctor* tmp_doctor;
				tmp_doctor = getVectorItem(doctors, d_num);
				doctor = tmp_doctor;

				cout << "Enter doctor's new full name (0 to skip this parametr): " << endl;
				str = checkNameFormat(DOC_UPDATE);
				if (str != "0") doctor.setName(str);

				data = choise(doctorRepo, DOC_SPEC_OPT, DOC_UPDATE);
				if (!data.empty()) {
					doctor.setSpecialization(data.at(0).first);
					data.clear();
				}

				data = choise(doctorRepo, DOC_CAB_OPT, DOC_UPDATE);
				if (!data.empty()) {
					doctor.setCabinet(data.at(0).first);
					data.clear();
				}

				data = choise(doctorRepo, DOC_PLOT_OPT, DOC_UPDATE);
				if (!data.empty()) {
					doctor.setPlot(atoi(data.at(0).first.c_str()));
					data.clear();
				}

				data = choise(doctorRepo, DOC_TT_OPT, DOC_UPDATE);
				if (!data.empty()) {
					doctor.addTimetableToCurrent(data);
					data.clear();
				}

				doctorRepo.edit(doctor);

				if (!data.empty()) 
					data.clear();

				doctors = doctorRepo.findAll();
				doctors = sortDoctorsTimetable(doctors);
			}

			break;
		}
		case 4: {
			if (doctorRepo.getMaxDoctorId() == 0) {
				cout << "List doctors is empty!" << endl << endl;
				break;
			}

			doctors = doctorRepo.findAll();
			d_num = choiseIntoListDoctor(doctors, "Choise doctor into list to delete (0 to exit): ");

			if (d_num > 0) {
				Doctor* tmp_doctor;
				tmp_doctor = getVectorItem(doctors, d_num);
				doctor = tmp_doctor;

				doctorRepo.remove(doctor);

				doctors = doctorRepo.findAll();
				doctors = sortDoctorsTimetable(doctors);
			}

			break;
			}
		case 5: {
			if (doctorRepo.getMaxDoctorId() == 0) {
				cout << "List doctors is empty!" << endl << endl;
				break;
			}

			doctors = doctorRepo.findAll();
			d_num = choiseIntoListDoctor(doctors, "Choise doctor into list to edit timetable (0 to exit): ");

			if (d_num > 0) {
				Doctor* tmp_doctor;
				tmp_doctor = getVectorItem(doctors, d_num);
				doctor = tmp_doctor;

				data = choise(doctorRepo, DOC_TT_OPT, DOC_UPDATE);
				if (!data.empty()) {
					doctor.addTimetableToCurrent(data);
					data.clear();
				}

				doctorRepo.edit(doctor);

				if (!data.empty())
					data.clear();

				doctors = doctorRepo.findAll();
				doctors = sortDoctorsTimetable(doctors);
			}

			break;
			}
		case 6: {
			if (doctorRepo.getMaxDoctorId() == 0) {
				cout << "List doctors is empty!" << endl << endl;
				break;
			}

			doctors = doctorRepo.findAll();
			doctors = sortDoctorsTimetable(doctors);
			d_num = choiseIntoListDoctor(doctors, "Choise doctor into list to remove timetable position (0 to exit): ");

			if (d_num > 0) {
				Doctor* tmp_doctor;
				tmp_doctor = getVectorItem(doctors, d_num);

				d_num_list = choiseIntoListTimetableDoctor(tmp_doctor, doctorRepo, "Choise timetable position to remove (0 to end chosing): ", DOC_INTO);
				doctor = tmp_doctor;

				if (!d_num_list.empty()) {
					doctorRepo.removeTimetablePosition(doctor, d_num_list);

					doctors = doctorRepo.findAll();
					doctors = sortDoctorsTimetable(doctors);
				}
			}
			
			break;
			}
		case 7: {
			if (doctorRepo.getMaxDoctorId() == 0) {
				cout << "List doctors is empty!" << endl << endl;
				break;
			}

			doctors = doctorRepo.findAll();
			doctors = sortDoctorsTimetable(doctors);
			d_num = choiseIntoListDoctor(doctors, "Choise doctor into list to show timetable (0 to exit): ");

			if(d_num > 0) {
				Doctor* tmp_doctor;
				tmp_doctor = getVectorItem(doctors, d_num);
				doctor = tmp_doctor;

				printDoctorTimetable(doctor,doctorRepo,DOC_ALL);
			}

			break;
			}
		
		case 8: {
			if (patientRepo.getMaxPatientId() == 0) {
				cout << "List patients is empty!" << endl << endl;
				break;
			}
			printPatients(patientRepo.findAll(), PAT_ALL);
			patients = patientRepo.findAll();
			break;
			}
		case 9: {
			cout << "Enter patient's full name: " << endl;
			str = checkNameFormat(PAT_INSERT);
			patient.setName(str);

			maxId = patientRepo.getMaxPatientId();
			if (maxId != 0) {
				patient.setId(maxId + 1);
			}
			else {
				patientRepo.alterSeqPatients(1);
				patient.setId(1);
			}

			cout << "Enter patient's address: " << endl;
			str = checkAddressFormat(PAT_INSERT);
			patient.setAddress(str);

			patientRepo.add(patient);

			patients = patientRepo.findAll();
			break;
			}
		case 10:{
			if (patientRepo.getMaxPatientId() == 0) {
				cout << "List patients is empty!" << endl << endl;
				break;
			}

			patients = patientRepo.findAll();
			d_num = choiseIntoListPatient(patients, "Choise patient into list to update (0 to exit): ");

			if (d_num > 0) {
				Patient* tmp_patient;
				tmp_patient = getVectorItem(patients, d_num);
				patient = tmp_patient;


				int tmpCount = 0;
				cout << "Enter patient's new full name (0 to skip this parametr): " << endl;
				str = checkNameFormat(PAT_UPDATE);
				if (str != "0") {
					patient.setName(str);
					tmpCount++;
				}

				cout << "Enter patient's address (0 to skip this parametr): " << endl;
				str = checkAddressFormat(PAT_UPDATE);
				if (str != "0") {
					patient.setAddress(str);
					tmpCount++;
				}

				if (tmpCount > 0) patientRepo.edit(patient);

				patients = patientRepo.findAll();
			}
			
			break;
			}
		case 11: {
			if (patientRepo.getMaxPatientId() == 0) {
				cout << "List patients is empty!" << endl << endl;
				break;
			}

			patients = patientRepo.findAll();
			d_num = choiseIntoListPatient(patients, "Choise patient into list to delete (0 to exit): ");

			if (d_num > 0) {
				Patient* tmp_patient;
				tmp_patient = getVectorItem(patients, d_num);
				patient = tmp_patient;

				patientRepo.remove(patient);

				patients = patientRepo.findAll();
			}

			break;
			}
		
		case 12: {
			bool isFinal = false;
			if (patientRepo.getMaxPatientId() == 0) {
				cout << "List patients is empty!" << endl << endl;
				break;
			}
			if (doctorRepo.getMaxDoctorId() == 0) {
				cout << "List doctors is empty!" << endl << endl;
				break;
			}

			if (receptionRepo.getMaxReceptionId() == 0) {
				cout << "List receptions is empty!" << endl << endl;
				break;
			}

			receptions = receptionRepo.findAll();

			while (true) {
				d_num = choiseIntoListReceptions(receptions, "Choise reception into list to details (0 to exit): ", REC_ALL);
				if (d_num <= 0) break;

				while (true) {
					printReception(receptions.at(d_num - 1), REC_ALL);
					cout << "View mode (0 to exit receptions menu)" << endl;
					cin >> d_num;
					if (d_num <= 0) {
						break;
					}
				}
			}
			
			break;
			}
		case 13: {
			bool isFinal = false;
			if (patientRepo.getMaxPatientId() == 0) {
				cout << "List patients is empty!" << endl << endl;
				break;
			}
			if (doctorRepo.getMaxDoctorId() == 0) {
				cout << "List doctors is empty!" << endl << endl;
				break;
			}

			maxId = receptionRepo.getMaxReceptionId();
			if (maxId != 0) {
				reception.setId(maxId + 1);
			}
			else {
				receptionRepo.alterSeqReceptions(1);
				reception.setId(1);
			}

			patients = patientRepo.findAll();
			doctors = doctorRepo.findAll();
			doctors = sortDoctorsTimetable(doctors);

			d_num = choiseIntoListPatient(patients, "Choise patient into list to registration for a reception (0 to exit): ");
			if (d_num == 0) break;

			Patient* tmp_patient;
			tmp_patient = getVectorItem(patients, d_num);
			patient = tmp_patient;

			d_num2 = choiseIntoListDoctor(doctors, "Choise doctor into list to treat a patient (0 to exit): ");
			if (d_num2 == 0) break;
			
			Doctor* tmp_doctor;
			tmp_doctor = getVectorItem(doctors, d_num2);
			doctor = tmp_doctor;

			int dayOfWeekNumber;
			while (true) {
				str = checkDateFormat(REC_INSERT);
				if (str != "0") reception.setDateOfReception(str);
				else break;

				dayOfWeekNumber = receptionRepo.whatDayOfWeek(str); // Получить идентификатор дня недели по дате
				
				timetablePosition = doctorRepo.checkJobAtDayOfWeek(tmp_doctor, dayOfWeekNumber); // Проверить, работает ли врач в данный день недели, и вернуть позицию распиания, если да

				if (timetablePosition.first == "0") {
					cout << "This is doctor don't work at this day of week!" << endl;
					continue;
				}
				else {
					//d_num_list = choiseIntoListTimetableDoctor(tmp_doctor, doctorRepo, "Choise timetable position to registration (0 to exit): ", DOC_ALL);
					//doctor = tmp_doctor;

					vector<pair<string, pair<string, string>>> tempTt;
					tempTt.push_back(timetablePosition);
					while (true) {
						str2 = checkTimeReception(tempTt);

						if (str2 == "0") break;

						if (!receptionRepo.checkEmployment(doctor, str2, str)) {
							reception.setTimeOfReception(str2);
							reception.setDoctor(doctor);
							reception.setPatient(patient);
							reception.setIsDone(false);

							receptionRepo.add(reception);
							isFinal = true;
							break;
						}
						else {
							cout << "This time is employment! Insert another!" << endl;
							continue;
						}
					}

					if (str2 == "0" || isFinal) break;
				}
			}

			//reception = receptionRepo.findAll();
			break;
			}
		case 14: {
			if (receptionRepo.getMaxReceptionId() == 0) {
				cout << "List receptions is empty!" << endl << endl;
				break;
			}

			receptions = receptionRepo.findAll();
			d_num = choiseIntoListReceptions(receptions, "Choise reception into list to add treatment (0 to exit): ", REC_INTO);

			if (d_num > 0) {
				Reception* tmp_reception;
				tmp_reception = getVectorItem(receptions, d_num);
				reception = tmp_reception;

				int tmpCount = 0;
				cout << "Enter medications with a space (0 to skip this parametr): " << endl;
				medications = checkMedicationsFormat();
				if (!medications.empty()) {
					tmpCount++;
					reception.setMedications(medications);
				}

				cout << "Enter procedures with a enter (0 to end written): " << endl;
				procedures = checkProceduresFormat();
				if (!procedures.empty()) {
					tmpCount++;
					reception.setProcedures(procedures);
				}

				while (true) {
					str = choiseAnalysis(receptionRepo.getAnalyses(), "Choise analysis into list add (0 to end choisen): ");
					if (str != "0") {
						tmpCount++;
						analyses.insert(pair<string,string>(str, "Not Passed"));
					}
					else break;
				}

				reception.setAnalyses(analyses);
				analyses.clear();

				if (tmpCount > 0) {
					receptionRepo.addTreatment(reception);
					receptions = receptionRepo.findAll();
				}
			}

			break;
		}
		case 15: {
			if (receptionRepo.getMaxReceptionId() == 0) {
				cout << "List receptions is empty!" << endl << endl;
				break;
			}

			receptions = receptionRepo.findAll();
			d_num = choiseIntoListReceptions(receptions, "Choise reception into list to edit (0 to exit): ", REC_INTO);

			// Внесение изменений приёма (жалобы + диагноз + смена врача (не закрытие)) (edit)
			if (d_num > 0) {
				Reception* tmp_reception;
				tmp_reception = getVectorItem(receptions, d_num);
				reception = tmp_reception;

				int tmpCount = 0;
				cout << "Enter the patient's complaints in any form (0 to skip this parametr): " << endl;
				str = checkComplaintsFormat(REC_UPDATE);
				if (str != "0") {
					reception.setComplaints(str);
					tmpCount++;
				}

				cout << "Enter the patient's diagnosis in any form (0 to skip this parametr): " << endl;
				str = checkDiagnosisFormat(REC_UPDATE);
				if (str != "0") {
					reception.setDiagnosis(str);
					tmpCount++;
				}

				cout << "Change doctor? (Yes - 1, No - 0) " << endl;
				str = "";
				//cin.ignore((numeric_limits<streamsize>::max)(), '\n');
				getline(cin, str);

				if (str == "1") {
					doctors = doctorRepo.findAll();
					d_num = choiseIntoListDoctor(doctors, "Choise doctor into list to change reception (0 to exit): ");

					if (d_num > 0) {
						tmpCount++;

						Doctor* tmp_doctor;
						tmp_doctor = getVectorItem(doctors, d_num);
						doctor = tmp_doctor;

						reception.setDoctor(doctor);
					}
				}

				int an_i = choiseAnalysis(receptions.at(d_num - 1), "Choise the patient's analysis to add analysis result (0 to skip this parametr): ");
				if (an_i > 0) {
					cout << "Enter the patient's analyses result in any form (0 to skip this parametr): " << endl;
					cin.ignore((numeric_limits<streamsize>::max)(), '\n');
					getline(cin, str);

					if (str != "0") {
						int ds = 0;
						analyses.clear();
						for (auto& item : reception.getAnalyses()) {
							if (ds == an_i - 1) {
								analyses.insert(pair<string, string>(item.first, str));
								continue;
							}
							analyses.insert(pair<string, string>(item.first, item.second));
							ds++;
						}
						reception.setAnalyses(analyses);

						tmpCount++;
					}
				}
				
				if (tmpCount > 0) {
					receptionRepo.edit(reception);
					receptions = receptionRepo.findAll();
				}
			}

			break;
			}
		case 16: {
			if (receptionRepo.getMaxReceptionId() == 0) {
				cout << "List receptions is empty!" << endl << endl;
				break;
			}

			receptions = receptionRepo.findAll();
			d_num = choiseIntoListReceptions(receptions, "Choise reception into list to extract (0 to exit): ", REC_INTO);

			if (d_num > 0) {
				if (!receptions.at(d_num - 1)->getIsDone()) {
					receptionRepo.setIsDone(receptions.at(d_num - 1));
				}
				else {
					cout << "Reception is closed!" << endl;
				}
			}

			break;
		}
		case 17: {
			if (receptionRepo.getMaxReceptionId() == 0) {
				cout << "List receptions is empty!" << endl << endl;
				break;
			}

			receptions = receptionRepo.findAll();
			d_num = choiseIntoListReceptions(receptions, "Choise reception into list to delete (0 to exit): ", REC_INTO);

			if (d_num > 0) {
				receptionRepo.remove(receptions.at(d_num - 1));
			}

			break;
		}

		}
		
	}
}

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	setlocale(LC_ALL, "");

	loop();

	return 0;
}