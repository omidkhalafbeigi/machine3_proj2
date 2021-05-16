#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <math.h>
#include <Windows.h>

using namespace std;

string single_comma(string File_Path)
{
    fstream reader;
    string buffer;
    string line_buffer;
    string data;
    int comma_iteraiton = 0;

    reader.open(File_Path);

    while (getline(reader, buffer))
    {
        for (char c : buffer)
        {
            if (c == ',')
            {
                comma_iteraiton += 1;
            }
            else
            {
                comma_iteraiton = 0;
            }

            if (comma_iteraiton > 1)
            {
                continue;
            }
            else
            {
                line_buffer += c;
            }
        }

        data += line_buffer;
        line_buffer = "";
        data += "\n";

        comma_iteraiton = 0;
    }
    reader.close();

    return data;
}

void save_data(string File_Path, string Text)
{
    ofstream writer(File_Path);
    writer << Text;
    writer.close();
}

float get_value(string file_path, int x, int y)
{
    string buffer;
    fstream reader;
    reader.open(file_path);

    for (int row = 1; row < x; row++)
    {
        getline(reader, buffer);
    }

    getline(reader, buffer, ',');

    for (int column = 1; column < y; column++)
    {
        getline(reader, buffer, ',');
    }

    reader.close();

    return stof(buffer);
}


int main()
{
    string file_name;
    char working_directory[MAX_PATH + 1];
    GetCurrentDirectoryA(sizeof(working_directory), working_directory);
    string current_path = working_directory;

    cout << "Copy file on this location (CSV Type): " << current_path << endl;

    cout << "Enter file name: ";
    cin >> file_name;

    string data = single_comma(current_path + "\\" + file_name);
    save_data(current_path + "\\" + "New_" + file_name, data);

    string file_path;
    file_path = current_path + "\\" + "New_" + file_name;

    // Get Ceiling Reflectance from user
    int Ceiling_reflectance = 10;
    cout << "Enter one of this numbers Ceiling reflectance {10, 30, 50, 70, 80, 90}: ";
    cin >> Ceiling_reflectance;

    // Get Wall Reflectance from user
    int Wall_reflectance = 10;
    cout << "Enter one of this numbers Wall reflectance {10, 30, 50, 70, 80, 90}: ";
    cin >> Wall_reflectance;

    // Get Floor Reflectance from user
    int Floor_reflectance = 10;
    float Floor = 0;
    cout << "Enter one of this numbers for Floor reflectance {10, 20, 30}: ";
    cin >> Floor_reflectance;

    switch (Floor_reflectance)
    {
    case 10:
        Floor = 0.975;
        break;
    case 20:
        Floor = 1;
        break;
    case 30:
        Floor = 1.05;
        break;
    default:
        cout << "Enter true number...";
        break;
    }

    // Reading value of "L" in csv file
    float L = get_value(file_path, 16, 2);
    // Reading value of "W" in csv file
    float W = get_value(file_path, 16, 5);
    // Reading value of "H" in csv file
    float H = get_value(file_path, 16, 8);

    float Luminaire_installation_height = get_value(file_path, 20, 3);
    float Workplane_height = get_value(file_path, 21, 3);

    // hcc calculation by values of "H" and "Luminaire_installation_height"
    float hcc = (H - Luminaire_installation_height);
    float hfc = Workplane_height;
    // hrc calculation
    float hrc = (H - hcc - hfc);
    // CCR calculation
    float CCR = (((5 * hcc * (L + W)) / L) / W);
    // RCR calculation
    float RCR = (((5 * hrc * (L + W)) / L) / W);
    // FCR calculation
    float FCR = (((5 * hfc * (L + W)) / L) / W);
    float pcc = get_value(file_path, 32, 3);
    float pfc = get_value(file_path, 35, 3);
    float CU = get_value(file_path, 36, 3);
    // Adjusted coefficient of utilization calculation
    float adjusted_CU = (CU * Floor);
    // Initialization of LLD
    float LLD = 0.79;

    // Get Fixture Type from user
    int fixture_type = 0;
    cout << "Enter number of one of this options {1: Incandescent, 2: Quartz, 3: Mercury, 4: Metal halide, 5: High pressure sodium, 6: Fluorescent}: ";
    cin >> fixture_type;

    switch (fixture_type)
    {
    case 1:
        LLD = 0.79;
        break;
    case 2:
        LLD = 0.88;
        break;
    case 3:
        LLD = 0.75;
        break;
    case 4:
        LLD = 0.69;
        break;
    case 5:
        LLD = 0.83;
        break;
    case 6:
        LLD = 0.76;
        break;
    default:
        cout << "Enter true number...";
        break;
    }

    // Reading Watts value
    float Watts = get_value(file_path, 42, 2);
    // Reading Lamps value
    float Lamps = get_value(file_path, 42, 4);
    // Reading Lumen value
    float Lumen = get_value(file_path, 42, 6);
    // Reading LDD value
    float LDD = get_value(file_path, 43, 4);
    // Total Light Loss calculation using "LDD" and "LLD" parameters
    float Total_light_loss = (LDD * LLD);

    // Reading Required Illuminance value
    float Required_illuminance = get_value(file_path, 47, 3);
    // Number_of_features calculation using "Required Rlluminance", "L", "W", "Lamps", "Lumen", "CU", "Total Light Loss values"
    float Number_of_features = (Required_illuminance * L * W) / (Lamps * Lumen * CU * Total_light_loss);
    // ROUNDUP "fixture type" value using ceil function in c++
    float Rounded_fixture = ceil(Number_of_features);
    // Number_of_features calculation using "Rounded Fixture", "L", "W", "Lamps", "Lumen", "Adjusted CU", "Total Light Loss values"
    float Resulted_illuminance = (Rounded_fixture * Lamps * Lumen * adjusted_CU * Total_light_loss) / (L * W);
    int pw = Wall_reflectance;

    string output_file = current_path + "\\" + "Output.csv";
    // By this function, we can save outputs as CSV file in project directory
    ofstream writer(output_file);

    cout << "L: " << L << endl;

    writer << "L";
    writer << ',';
    writer << L;
    writer << "\n";

    cout << "W: " << W << endl;

    writer << "W";
    writer << ',';
    writer << W;
    writer << "\n";

    cout << "H: " << H << endl;

    writer << "H";
    writer << ',';
    writer << H;
    writer << "\n";

    cout << "Luminaire_installation_height: " << Luminaire_installation_height << endl;

    writer << "Luminaire_installation_height";
    writer << ',';
    writer << Luminaire_installation_height;
    writer << "\n";

    cout << "Workplane_height: " << Workplane_height << endl;

    writer << "Workplane_height";
    writer << ',';
    writer << Workplane_height;
    writer << "\n";

    cout << "hcc: " << hcc << endl;

    writer << "hcc";
    writer << ',';
    writer << hcc;
    writer << "\n";

    cout << "hfc: " << hfc << endl;

    writer << "hfc";
    writer << ',';
    writer << hfc;
    writer << "\n";

    cout << "hrc: " << hrc << endl;

    writer << "hrc";
    writer << ',';
    writer << hrc;
    writer << "\n";

    cout << "CCR: " << CCR << endl;

    writer << "CCR";
    writer << ',';
    writer << CCR;
    writer << "\n";

    cout << "RCR: " << RCR << endl;

    writer << "RCR";
    writer << ',';
    writer << RCR;
    writer << "\n";

    cout << "FCR: " << FCR << endl;

    writer << "FCR";
    writer << ',';
    writer << FCR;
    writer << "\n";

    cout << "pcc: " << pcc << endl;

    writer << "pcc";
    writer << ',';
    writer << pcc;
    writer << "\n";

    cout << "pw: " << pw << endl;

    writer << "pw";
    writer << ',';
    writer << pw;
    writer << "\n";

    cout << "pfc: " << pfc << endl;

    writer << "pfc";
    writer << ',';
    writer << pfc;
    writer << "\n";

    cout << "CU: " << CU << endl;

    writer << "CU";
    writer << ',';
    writer << CU;
    writer << "\n";

    cout << "adjusted_CU: " << adjusted_CU << endl;

    writer << "adjusted_CU";
    writer << ',';
    writer << adjusted_CU;
    writer << "\n";

    cout << "LLD: " << LLD << endl;

    writer << "LLD";
    writer << ',';
    writer << LLD;
    writer << "\n";

    cout << "fixture_type: " << fixture_type << endl;

    writer << "fixture_type";
    writer << ',';
    writer << fixture_type;
    writer << "\n";

    cout << "Watts: " << Watts << endl;

    writer << "Watts";
    writer << ',';
    writer << Watts;
    writer << "\n";

    cout << "Lamps: " << Lamps << endl;

    writer << "Lamps";
    writer << ',';
    writer << Lamps;
    writer << "\n";

    cout << "Lumen: " << Lumen << endl;

    writer << "Lumen";
    writer << ',';
    writer << Lumen;
    writer << "\n";

    cout << "LDD: " << LDD << endl;

    writer << "LDD";
    writer << ',';
    writer << LDD;
    writer << "\n";

    cout << "Total_light_loss: " << Total_light_loss << endl;

    writer << "Total_light_loss";
    writer << ',';
    writer << Total_light_loss;
    writer << "\n";

    cout << "Required_illuminance: " << Required_illuminance << endl;

    writer << "Required_illuminance";
    writer << ',';
    writer << Required_illuminance;
    writer << "\n";

    cout << "Number_of_features: " << Number_of_features << endl;

    writer << "Number_of_features";
    writer << ',';
    writer << Number_of_features;
    writer << "\n";

    cout << "Rounded_fixture: " << Rounded_fixture << endl;

    writer << "Rounded_fixture";
    writer << ',';
    writer << Rounded_fixture;
    writer << "\n";

    cout << "Resulted_illuminance: " << Resulted_illuminance << endl;

    writer << "Resulted_illuminance";
    writer << ',';
    writer << Resulted_illuminance;
    writer << "\n";

    writer.close();

    cout << endl << endl << "File saved on " + current_path << endl << endl;

    cin;
}
