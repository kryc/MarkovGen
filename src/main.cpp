#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>

// Function to generate the weights file
void
build(
    std::string const& file
)
{
    size_t counts[256][256];
    double weights[256][256];
    // Initialize the counts
    memset(counts, 0, sizeof(counts));
    // Initialize the weights
    memset(weights, 0, sizeof(weights));
    // Read each line from stdin
    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
        {
            continue;
        }
        // Store the initial character count
        counts[0][line[0]]++;
        // Count the frequency of each character pair
        for (size_t i = 0; i < line.size() - 1; ++i)
        {
            counts[line[i]][line[i + 1]]++;
        }
    }
    // Calculate the weights
    for (size_t i = 0; i < 256; ++i)
    {
        size_t total = 0;
        for (size_t j = 0; j < 256; ++j)
        {
            total += counts[i][j];
        }
        for (size_t j = 0; j < 256; ++j)
        {
            weights[i][j] = static_cast<double>(counts[i][j]) / total;
        }
    }
    // Write the weights to the file
    FILE* f = fopen(file.c_str(), "wb");
    if (f == nullptr)
    {
        std::cerr << "Failed to open file " << file << std::endl;
        return;
    }
    fwrite(weights, sizeof(double), 256 * 256, f);
    fclose(f);
}

// Function to output a report of the weights
void
report(
    std::string const& file
)
{
    double weights[256][256];
    // Initialize the wieghts
    memset(weights, 0, sizeof(weights));

    FILE* f = fopen(file.c_str(), "rb");
    if (f == nullptr)
    {
        std::cerr << "Failed to open file " << file << std::endl;
        return;
    }
    fread(weights, sizeof(double), 256 * 256, f);
    fclose(f);
    std::cout << "***Start Character***" << std::endl;
    for (size_t j = ' '; j <= '~'; ++j)
    {
        if (weights[0][j] > 0)
        {
            std::cout << std::fixed << std::setprecision(5) << static_cast<char>(j) << " " << weights[0][j] << std::endl;
        }
    }
    std::cout << "***Non-start Characters***" << std::endl;
    for (size_t i = ' '; i <= '~'; ++i)
    {
        for (size_t j = ' '; j <= '~'; ++j)
        {
            if (weights[i][j] > 0)
            {
                std::cout << static_cast<char>(i) << static_cast<char>(j) << " " << weights[i][j] << std::endl;
            }
        }
    }
}

bool tick_position(
    char buffer[1024],
    size_t length,
    size_t position,
    double min_weight,
    double weights[256][256]
)
{
    for (size_t j = buffer[position] + 1; j <= '~'; ++j)
    {
        if (position == 0 && weights[0][j] >= min_weight)
        {
            buffer[position] = static_cast<char>(j);
            return true;
        }
        else if (position > 0 && weights[buffer[position - 1]][j] >= min_weight)
        {
            buffer[position] = static_cast<char>(j);
            return true;
        }
    }
    return false;
}

bool
tick(
    char buffer[1024],
    size_t length,
    double min_weight,
    double weights[256][256]
)
{
    // Try and replace the last character with the next
    // character based on the previous character
    for (ssize_t i = length - 1; i > -1; --i)
    {
        if (tick_position(buffer, length, i, min_weight, weights))
        {
            // If it is not the last character, reset all subsequent
            // characters to the next character based on the previous
            // character
            if (i < length - 1)
            {
                for (size_t j = i + 1; j < length; ++j)
                {
                    for (size_t k = ' '; k <= '~'; ++k)
                    {
                        if (weights[buffer[j - 1]][k] >= min_weight)
                        {
                            buffer[j] = static_cast<char>(k);
                            break;
                        }
                    }
                }
            }
            return true;
        }
    }
    return false;
}

// Generate strings based on the weights
void
generate(
    std::string const& file,
    size_t length,
    double min_weight
)
{
    double weights[256][256];
    // Initialize the weights
    memset(weights, 0, sizeof(weights));

    FILE* f = fopen(file.c_str(), "rb");
    if (f == nullptr)
    {
        std::cerr << "Failed to open file " << file << std::endl;
        return;
    }
    fread(weights, sizeof(double), 256 * 256, f);
    fclose(f);
    // Generate and print all strings
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    // For each position in length, find the first
    // character based above the minimum weight
    for (size_t i = 0; i < length; ++i)
    {
        if (i == 0)
        {
            for (size_t j = ' '; j <= '~'; ++j)
            {
                if (weights[0][j] >= min_weight)
                {
                    buffer[0] = static_cast<char>(j);
                    break;
                }
            }
            if (buffer[0] == '\0')
            {
                std::cerr << "No characters above minimum weight" << std::endl;
                return;
            }
        }
        else
        {
            // For each subsequent position, find the next
            // character based on the previous character
            for (size_t j = 1; j < length; ++j)
            {
                for (size_t k = ' '; k <= '~'; ++k)
                {
                    if (weights[buffer[j - 1]][k] >= min_weight)
                    {
                        buffer[j] = static_cast<char>(k);
                        break;
                    }
                }
            }
        }
    }
    std::cout << buffer << std::endl;
    while (tick(buffer, length, min_weight, weights))
    {
        std::cout << buffer << std::endl;
    }
}

// Main function
int main(
    int argc,
    char* argv[]
)
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " mode file" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    std::string file = argv[2];
    if (mode == "build")
    {
        build(file);
    }
    else if (mode == "report")
    {
        report(file);
    }
    else if (mode == "generate")
    {
        if (argc < 4)
        {
            std::cerr << "Usage: " << argv[0] << "generate file minlength maxlength min_weight" << std::endl;
            return 1;
        }
        size_t minlength = std::stoi(argv[3]);
        size_t maxlength = std::stoi(argv[4]);
        double min_weight = std::stod(argv[5]);
        for (size_t length = minlength; length <= maxlength; ++length)
        {
            generate(file, length, min_weight);
        }
    }
    else
    {
        std::cerr << "Invalid mode " << mode << std::endl;
        return 1;
    }
}