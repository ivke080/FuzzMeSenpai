#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <regex>
#include <thread>
#include <vector>
#include <map>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>

void fuzz(const std::string& url, const std::string& file)
{
    std::cout << std::this_thread::get_id() << std::endl;
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
                std::cout << url + end + line << std::endl;
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

    std::map<std::string, std::string> wordFiles = {
        {"-c", "./fuzzdb/discovery/predictable-filepaths/dicc.txt"},
        {"-w", "./fuzzdb/discovery/predictable-filepaths/cms/wordpress.txt"},
        {"-j", "./fuzzdb/discovery/predictable-filepaths/cms/joomla_plugins.txt"},
        {"-d", "./fuzzdb/discovery/predictable-filepaths/cms/drupal_plugins.txt"}
    };

    std::vector<std::thread> threads;

    std::regex reg("(http[s]?):\/\/www\.[a-z0-9]+([\-\.]{1}[a-z0-9]+)*\.[a-z]{2,5}(:[0-9]{1,5})?(\/.*)?");
    
    if (argc < 3) 
    {
        std::cerr << "Usage: ./fuzz url [-c] [-w] [-j] [-d]\n";
    }
    else if (std::regex_match(argv[1], reg) == false)
    {
        std::cerr << "Usage: ./fuzz url [-c] [-w] [-j] [-d]\nInvalid URL (format: http://www.example.com)\n";
    }
    else
    {
        for (size_t i = 2; i < argc; i++) {
            threads.emplace_back(fuzz, argv[1], wordFiles[argv[i]]);
        }
    }

    for (std::thread& t : threads)
        t.join();
    
    return 0;
}