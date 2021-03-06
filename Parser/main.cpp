#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <boost/lexical_cast.hpp>

struct Author
{
	std::string surname;
	std::string name_pathronymic;

	bool operator==(const Author& a)
	{
		if (this->surname == a.surname && this->name_pathronymic == a.name_pathronymic)
			return true;
		return false;
	}
};

struct Publication
{
	std::string type1;
	std::string type2;
	std::string title;
	std::string year_pub;
	std::string pub_house;
	std::string place_pub;
	std::vector<Author> authors;
	std::string url;
	std::string pages;
	std::string udk;
	std::string isbn;

	void print(std::ostream& os = std::cout);
};

/*разделение общей строки на массив строк, разделитель - запятая*/
std::vector<std::string> delim_string(const std::string& to_delim);

/*парсит строку, создает и заполняет публикацию
возвращает - заполнено ли название публикации
*/
bool parse_vector(const std::vector<std::string>& arr, std::vector<Publication>& pub_arr);

/*если возникает ситуация, что название публикации не записано функцией выше
возвращает - заполнено ли название публикации
*/
bool parse_vector_deferred(const std::vector<std::string>& arr, std::vector<Publication>& pub_arr);

int main(int argc, char* argv[])
{
	system("chcp 65001>nul");

	if (argc != 3)
	{
		std::cout << "Недостаточно аргументов. Вид команды: имя_программы какой_файл_парсить файл_с_результатами" << std::endl;
		return 0;
	}

	std::ifstream ifs(argv[1]);
	if (!ifs.is_open())
	{
		std::cout << "Не удалось открыть файл, который нужно парсить" << std::endl;
		return 0;
	}

	std::vector<Publication> pub_arr;
	std::string input{};
	std::getline(ifs, input);
	std::getline(ifs, input);

	size_t counter{0};
	bool is_title_write{ true };
	while (std::getline(ifs, input))
	{
		std::string num{};
		std::vector<std::string> arr{ delim_string(input) };

		if (is_title_write && input.size() > 8)
		{
			for (size_t i = 0; i < 8; i++)
			{
				if (input[i] >= '0' && input[i] <= '9')
				{
					num.append(1, input[i]);
				}
			}
			if (num.size() == 8) /*опознаем начало информации о публикации по id - восьмизначному*/
			{
				/*if (arr[1] == "https://elibrary.ru/item.asp?id=42531043")
				{
					std::cout << counter << std::endl;
					for (size_t i = 0; i < arr.size(); i++)
					{
						std::cout << i << ": " << arr[i] << std::endl;
					}
				}*/

				is_title_write = parse_vector(arr, pub_arr);
			}
		}
		is_title_write = parse_vector_deferred(arr, pub_arr);

		for (size_t i = 0; i < arr.size(); i++)
		{
			if (arr[i].length() == 13)
			{
				num = {};
				for (size_t j = 0; j < 13; j++)
				{
					if (arr[i][j] >= '0' && arr[i][j] <= '9')
					{
						num.append(1, arr[i][j]);
					}
				}
				if (num.size() == 13)
				pub_arr[pub_arr.size() - 1].isbn = arr[i];
			}
		}

		/*if (counter == 9376)
		{
			for (size_t i = 0; i < arr.size(); i++)
			{
				std::cout << i << ": " << arr[i] << std::endl;
			}
		}*/

		counter++;
	}
	ifs.close();

	std::ofstream ofs(argv[2]);
	if (!ofs.is_open())
	{
		std::cout << "Не удалось открыть файл, в который нужно записать результаты" << std::endl;
		return 0;
	}

	for (size_t i = 0; i < pub_arr.size(); i++)
	{
		ofs << i << ": " << std::endl;
		pub_arr[i].print(ofs);
		ofs << std::endl;
	}
	ofs.close();

	/*for (auto& i : pub_arr)
	{
		if (i.title == "3")
		{
			i.print();
			std::cout << std::endl;
		}
	}*/
	/*int len_min{ INT_MAX };
	Publication* p{};
	for (auto& i : pub_arr)
	{
		if (i.title.length() <= len_min)
		{
			p = & i;
			len_min = i.title.length();
		}
	}
	if (p)
		p->print();*/

	return bool(std::cout);
}

std::vector<std::string> delim_string(const std::string& to_delim)
{
	std::vector<std::string> arr;
	std::string delim(",");
	size_t prev = 0;
	size_t next;
	size_t delta = delim.length();

	while ((next = to_delim.find(delim, prev)) != std::string::npos)
	{
		arr.push_back(to_delim.substr(prev, next - prev));
		prev = next + delta;
	}
	arr.push_back(to_delim.substr(prev));
	int i{};
	for (i = arr.size() - 1; i >= 0; i--)
	{
		if (arr[i].length())
			break;
	}
	/*if (arr[0] == "обеспечение качества деталей")
	{
		std::cout << to_delim << std::endl;
		for (size_t i = 0; i < arr.size(); i++)
		{
			std::cout << i << ": " << arr[i] << std::endl;
		}
	}*/

	if (i < 0) i = 0;
	arr.erase(arr.begin() + i + 1, arr.end());
	for (size_t i = 0; i < arr.size(); i++)
	{
		int count_qm{ int(std::count(arr[i].begin(), arr[i].end(), '\"')) };
		if (count_qm % 2 != 0 && arr[i][0] == '\"')
		{
			std::string tmp{ arr[i] };
			int j = i + 1;
			int count_qm_find{};
			for (; j < arr.size(); j++)
			{
				count_qm_find = std::count(arr[j].begin(), arr[j].end(), '\"');
				tmp += arr[j];
				if (count_qm_find % 2 != 0)
				{
					break;
				}
			}
			if (j == arr.size()) j -= 1;
			arr.erase(arr.begin() + i, arr.begin() + j + 1);
			arr.insert(arr.begin() + i, tmp);
		}
	}
	/*if (arr[0] == "обеспечение качества деталей")
	{
		for (size_t i = 0; i < arr.size(); i++)
		{
			std::cout << i << ": " << arr[i] << std::endl;
		}
		std::cout << arr.size() << std::endl;
	}*/

	return arr;
}

bool parse_vector(const std::vector<std::string>& arr, std::vector<Publication>& pub_arr)
{
	Publication p;
	Author a;

	/*информация о книге*/
	p.url = arr[1];
	p.type1 = arr[2];
	p.type2 = arr[3];
	p.year_pub = arr[6];
	if (!p.year_pub.length())
	{
		for (int i = arr.size() - 1; i >= 0; i--)
		{
			if (arr[i] == "no")
			{
				p.year_pub = arr[i + 1];
				break;
			}
		}
	}

	p.title = arr[8];
	if (!p.title.length())
	{
		for (int i = arr.size() - 1; i >= 0; i--)
		{
			if (arr[i] == "RU" && arr[i - 1].length() >= 10)
			{
				p.title = arr[i - 1];
				break;
			}
		}
	}

	p.place_pub = arr[13];
	p.pages = arr[14].length() ? arr[14] : arr[4];
	p.pub_house = arr[15];

	/*информация об авторах*/
	int counter{ 1 };
	for (size_t i = 16; i < arr.size() - 3; i++)
	{
		if (arr[i] == std::to_string(counter) && arr[i + 1] == "RU" && arr[i + 2].length() < 48)
		{
			a.surname = arr[i + 2];
			a.name_pathronymic = arr[i + 3];
			p.authors.push_back(a);
			counter++;
		}
	}
	/*поиск удк и издательства*/
	for (size_t i = 16; i < arr.size(); i++)
	{
		if (arr[i] == std::string("УДК"))
		{
			p.udk = arr[i - 1];
		}
	}

	if (!p.authors.size()) /*если нет авторов русских*/
	{
		for (size_t i = 16; i < arr.size() - 3; i++)
		{
			if (arr[i] == std::to_string(counter) && arr[i + 1] == "EN")
			{
				a.surname = arr[i + 2];
				a.name_pathronymic = arr[i + 3];
				p.authors.push_back(a);
				counter++;
			}
		}
	}
	if (!p.authors.size()) /*если авторы не с первого*/
	{
		int counter{ -1 };
		for (size_t i = 16; i < arr.size() - 3; i++)
		{
			if (counter < 0)
			{
				try
				{
					int lex = boost::lexical_cast<int>(arr[i]);
					if (arr[i + 1] == "RU")
					{
						counter = lex;
						a.surname = arr[i + 2];
						a.name_pathronymic = arr[i + 3];
						p.authors.push_back(a);
						counter++;
					}
				}
				catch (...)
				{
					continue;
				}
			}
			else
			{
				if (arr[i] == std::to_string(counter) && arr[i + 1] == "RU")
				{
					a.surname = arr[i + 2];
					a.name_pathronymic = arr[i + 3];
					p.authors.push_back(a);
					counter++;
				}
			}
		}
	}

	pub_arr.push_back(p);
	return bool(p.title.length());
}

bool parse_vector_deferred(const std::vector<std::string>& arr, std::vector<Publication>& pub_arr)
{
	if (arr.size() > 76)
	{
		if (!pub_arr[pub_arr.size() - 1].title.length() && arr.size() > 132)
		{
			if (arr[132].size() >= 10)
				pub_arr[pub_arr.size() - 1].title = arr[132];
			else
				pub_arr[pub_arr.size() - 1].title = arr[136];
		}
		int counter = pub_arr[pub_arr.size() - 1].authors.size() + 1;
		/*if (arr[0] == "обеспечение качества деталей")
		{
			std::cout << pub_arr[pub_arr.size() - 1].authors.size() + 1 << std::endl;
		}*/
		Author a;
		for (size_t i = 0; i < arr.size() - 3; i++)
		{
			if (arr[i] == std::to_string(counter) && arr[i + 1] == "RU")
			{
				a.surname = arr[i + 2];
				a.name_pathronymic = arr[i + 3];
				if (std::find(pub_arr[pub_arr.size() - 1].authors.begin(), pub_arr[pub_arr.size() - 1].authors.end(), a) == pub_arr[pub_arr.size() - 1].authors.end())
					pub_arr[pub_arr.size() - 1].authors.push_back(a);
				counter++;
			}
		}
		if (!pub_arr[pub_arr.size() - 1].authors.size())
		{
			for (size_t i = 0; i < arr.size() - 3; i++)
			{
				if (arr[i] == std::to_string(counter) && arr[i + 1] == "EN")
				{
					a.surname = arr[i + 2];
					a.name_pathronymic = arr[i + 3];
					if (std::find(pub_arr[pub_arr.size() - 1].authors.begin(), pub_arr[pub_arr.size() - 1].authors.end(), a) == pub_arr[pub_arr.size() - 1].authors.end())
						pub_arr[pub_arr.size() - 1].authors.push_back(a);
					counter++;
				}
			}
		}
	}
	return bool(pub_arr[pub_arr.size() - 1].title.length());
}

void Publication::print(std::ostream& os)
{
	os << "Название: " << title << std::endl;
	os << "Тип: " << type1 << ", " << type2 << std::endl;
	os << "Издательство: " << pub_house << ", год: " << year_pub << ", место: " << place_pub << std::endl;
	os << "Количество страниц: " << pages << std::endl;
	os << "Автор(ы):" << std::endl;
	for (size_t i = 0; i < authors.size(); i++)
	{
		os << authors[i].surname << " " << authors[i].name_pathronymic;
		if (i != authors.size() - 1) os << ";";
	}
	os << std::endl;
	os << "Ссылка на издание: " << url << std::endl;
	if (udk.length()) os << "УДК: " << udk << std::endl;
	if (isbn.length()) os << "ISBN: " << isbn << std::endl;
}
