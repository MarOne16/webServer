#include "./webserver.hpp"

void Response::is_path_outside_directoryy(std::string path, std::string directory)
{
    char abs_path[PATH_MAX];
    char abs_directory[PATH_MAX];
    // Get the absolute paths of the given path and directory
    while (findFile(path) == 0 && path != "/")
    {
        path = path.substr(0, path.rfind('/'));
    }
    if (realpath(path.c_str(), abs_path) == NULL || realpath(directory.c_str(), abs_directory) == NULL)
    {
        this->status = 400;
        return;
    }
    if (strncmp(abs_path, abs_directory, strlen(abs_directory)) != 0)
    {
        this->status = 400;
    }
}

std::string Response::get_Content_type(std::string url)
{

    std::string contentTypes[25] = {
        "text/plain",
        "text/html",
        "text/css",
        "text/javascript",
        "text/xml",
        "application/json",
        "application/xml",
        "application/xhtml+xml",
        "application/pdf",
        "application/octet-stream",
        "image/jpeg",
        "image/png",
        "image/jpg",
        "image/gif",
        "image/svg+xml",
        "audio/mpeg",
        "audio/wav",
        "video/mp4",
        "video/webm",
        "multipart/form-data",
        "multipart/byteranges",
        "application/x-www-form-urlencoded",
        "application/graphql",
        "application/vnd.api+json",
    };
    std::string extension = url.substr(url.rfind(".") + 1);
    unsigned int i = 0;
    while (i < 25)
    {
        if (contentTypes[i].find(extension) != std::string::npos)
            return contentTypes[i];
        i++;
    }

    return "text/html;";
}

int Response::get_permission(std::string &file)
{
    if (access(file.c_str(), F_OK) == -1)
        return -1;
    if (access(file.c_str(), R_OK) == -1)
        return -2;
    return 0;
}

std::string Response::read_file(const std::string &filename)
{

    std::ifstream file(filename);
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    std::string fileContent;
    fileContent.resize(static_cast<size_t>(fileSize), '\0');
    if (file.is_open())
    {
        file.read(&fileContent[0], fileSize);
        file.close();
        return fileContent;
    }
    this->status = 400;
    file.close();
    return " ";
}

void Response::ft_default_pages(std::string status, std::string &message, std::string &path)
{
    struct stat buffer;
    std::string filename;
    if (stat(this->response_items.error_pages.find(status)->second.c_str(), &buffer) != -1)
    {
        filename = path;
        message = this->read_file(filename);
    }
}