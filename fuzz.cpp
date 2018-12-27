#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <regex>
#include <thread>
#include <vector>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>

enum WordFiles
{
    Common = 0,
    Wordpress = 1,
    Joomla = 2,
    Drupal = 3
};

std::vector<std::string> fuzzed;

void fuzz(const std::string& url, const std::string& file)
{
    try
    {
        curlpp::Cleanup cleanup;
        curlpp::Easy request;
        std::ostringstream os;
        curlpp::options::WriteStream ws(&os);

        std::ifstream f(file);
        std::string line;
        
        std::string end = url[url.size() - 1] != '/' ? "/" : "";
        
        request.setOpt(ws);

        while (std::getline(f, line)) {
            request.setOpt<curlpp::options::Url>(url + end + line);
            request.perform();

            if (curlpp::infos::ResponseCode::get(request) == 200)
            {
                fuzzed.push_back(url + end + line);
            }
        }
    }
    catch (curlpp::RuntimeError& e)
    {
        std::cout << e.what() << std::endl;
    }
    catch(curlpp::LogicError& e)
    {
        std::cout << e.what() << std::endl;
    }
}

int main(int argc, char *argv[])
{

    std::string wordFiles[] = {
        "./fuzzdb/discovery/predictable-filepaths/dicc.txt",
        "./fuzzdb/discovery/predictable-filepaths/cms/wordpress.txt",
        "./fuzzdb/discovery/predictable-filepaths/cms/joomla_plugins.txt",
        "./fuzzdb/discovery/predictable-filepaths/cms/drupal_plugins.txt"
    };

    std::regex reg("(http[s]?):\/\/www\.[a-z0-9]+([\-\.]{1}[a-z0-9]+)*\.[a-z]{2,5}(:[0-9]{1,5})?(\/.*)?");
    
    if (argc < 2) 
    {
        std::cerr << "Usage: ./fuzz url [-w] [-j] [-d]\n";
    }
    else if (std::regex_match(argv[1], reg) == false)
    {
        std::cerr << "Usage: ./fuzz url [-w] [-j] [-d]\nInvalid URL (format: http://www.example.com)\n";
    }
    else
    {
        fuzz(argv[1], wordFiles[Common]);
    }


    for (const std::string& url : fuzzed)
    {
        std::cout << url << std::endl;
    }

    return 0;
}