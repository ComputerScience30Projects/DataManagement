#include <sqlite3.h> 
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

struct User
{
    int id;
    std::string username;
};

struct Fragrance
{
    int id;
    unsigned int price;
    std::string name, fragrance, strength;
};

User user;

std::vector<Fragrance> fragrances;
std::vector<unsigned int> favorites;

static int callbackFragrances(void *data, int argc, char **argv, char **azColName)
{
    for(size_t i = 0; i < argc; i++)
    { 
        int index = fragrances.size() - 1;

        if (strcmp(azColName[i], "id") == 0)
        {
            fragrances.push_back(Fragrance());       
            index = fragrances.size() - 1;
            fragrances[index].id = atoi(argv[i]);
        }

        else if (strcmp(azColName[i], "name") == 0)
            fragrances[index].name = argv[i];

        else if (strcmp(azColName[i], "fragrance") == 0)
            fragrances[index].fragrance = argv[i];

        else if (strcmp(azColName[i], "strength") == 0)
            fragrances[index].strength = argv[i];

        else if (strcmp(azColName[i], "price") == 0)
            fragrances[index].price = atoi(argv[i]);
    }

    return 0;
}

static int callbackUsers(void *data, int argc, char **argv, char **azColName)
{
    for(size_t i = 0; i < argc; i++)
        if (strcmp(azColName[i], "id") == 0)
            user.id = atoi(argv[i]);

    return 0;
}

static int callbackFavorites(void *data, int argc, char **argv, char **azColName)
{
    for(size_t i = 0; i < argc; i++)
        if (strcmp(azColName[i], "favorite") == 0)
            favorites.push_back(atoi(argv[i]));

    return 0;
}


void createLogin(sqlite3* p_db)
{
    std::string username, password;
    int rc;
    char* errorMsg = 0;

    /* Get User Login */
    std::cout << "Username: ";
    std::getline(std::cin, username);
    std::cout << std::endl;

    std::cout << "Password: ";
    std::getline(std::cin, password);
    std::cout << std::endl;

    /* Create SQL statement */
    std::string sql = "INSERT INTO users (username, password) VALUES ('" + username + "','" + password + "');";

    /* Execute SQL statement */
    fragrances.clear();
    rc = sqlite3_exec(p_db, sql.c_str(), NULL, 0, &errorMsg);

    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    }
    else
        user.username = username;
        std::cout << "Account " + username << " was successfully created." << std::endl;
}

bool checkLogin(sqlite3* p_db)
{
    std::string username, password;
    int rc;
    char* errorMsg = 0;

    /* Get User Login */
    std::cout << "Username: ";
    std::getline(std::cin, username);
    std::cout << std::endl;

    std::cout << "Password: ";
    std::getline(std::cin, password);
    std::cout << std::endl;

    /* Create SQL statement */
    std::string sql = "SELECT * FROM 'users' WHERE username='" + username + "' AND password='" + password + "';";

    /* Execute SQL statement */
    struct sqlite3_stmt *selectstmt;
    rc = sqlite3_prepare_v2(p_db, sql.c_str(), -1, &selectstmt, NULL);

    if(rc == SQLITE_OK)
    {
        if (sqlite3_step(selectstmt) == SQLITE_ROW)
        {
            user.username = username;
            std::cout << "Successfully logged in as " + username << "." << std::endl;
        }
        else
        {
            std::cout << "Invalid login please try again." << std::endl << std::endl;
            return false;
        }
    }
    else
    {
        std::cout << "SQL error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);    
    }

    rc = sqlite3_exec(p_db, sql.c_str(), callbackUsers, 0, &errorMsg);

    return true;
}

void searchFragrances(sqlite3* p_db)
{
    int rc;
    char* errorMsg = 0;
    size_t choice = 0;


    std::cout << "|-----SEARCH FRAGRANCES-----|" << std::endl;

    /* Create SQL statement */
    std::string sql = "SELECT * FROM 'fragrances' ";

    /* Select Search Method */    
    std::cout << std::endl << "Choose searching method:\n1. ID\n2. Name\n3. Smell\n4. Display all fragrances";
    std::cout << std::endl << "Choose: ";

    do{
        std::cin >> choice;
        std::cin.ignore(); //Clear for getline

        switch(choice)
        {
            case 1:
                sql += "WHERE id";
                break;

            case 2:
                sql += "WHERE name";
                break;

            case 3:
                sql += "WHERE fragrance";
                break;

            case 4:
                break;
                
            default:
                choice = 0;
                break;               
        }
    }while (choice == 0);

    /* Define Search Term */
    if (choice != 4)
    {    
        std::string searchTerm;
        std::cout << "Search: ";
        std::getline(std::cin, searchTerm);

        if (choice == 1)
            sql += " LIKE '" + searchTerm + "'";
        else
            sql += " LIKE '%" + searchTerm + "%'";
    }

    /* Execute SQL Statement */
    fragrances.clear();
    rc = sqlite3_exec(p_db, sql.c_str(), callbackFragrances, 0, &errorMsg);

    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    } 

    /* Display Fragrances */
    for (size_t i = 0; i < fragrances.size(); i++)
        std::cout << std::endl <<
        "id:" << fragrances[i].id << std::endl << 
        "name:" << fragrances[i].name << std::endl << 
        "fragrance:" << fragrances[i].fragrance << std::endl << 
        "strength:" << fragrances[i].strength << std::endl <<
        "price:" << fragrances[i].price << std::endl << std::endl;
}

void addFragrance(sqlite3* p_db)
{
    int rc;
    char* errorMsg = 0;

    size_t choice = 0;
    std::string name, fragrance, strength;
    int price;

    /* Input Data */
    std::cout << "|-----ADDING FRAGRANCE-----|\nName: ";
    std::getline(std::cin, name);

    std::cout << std::endl << "Fragrance: ";
    std::getline(std::cin, fragrance);
    
    std::cout << std::endl << "1. Perfume(20-30%)\n2. Eau de Parfum(15-20%)\n3. Eau de Toillette(5-15%)\n4. Eau de Cologne(2-4%)\n5. Eau Fraiche(1-3%)";
    std::cout << std::endl << "Strength: ";

    
    do{
        std::cin >> choice;

        switch(choice)
        {
            case 1:
                strength = "Perfume(20-30%)";
                break;

            case 2:
                strength = "Eau de Parfum(15-20%)";
                break;

            case 3:
                strength = "Eau de Toillette(5-15%)";
                break;

            case 4:
                strength = "Eau de Cologne(2-4%)";
                break;

            case 5:
                strength = "Eau Fraiche(1-3%)";
                break;

            default:
                choice = 0;
                break;               
        }
    }while (choice == 0);

    
    std::cout << std::endl << "Price: ";
    std::cin >> price;

    /* Create SQL statement */
    std::string sql = "INSERT INTO fragrances (name , fragrance, strength, price) VALUES ('" + name + "', '" + fragrance + "', '" + strength + "', " + std::to_string(abs(price)) + ");";

    /* Execute SQL statement */
    fragrances.clear();
    rc = sqlite3_exec(p_db, sql.c_str(), NULL, 0, &errorMsg);

    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    } 
    else 
        std::cout << "Fragrance \"" << name << "\" was added to the database" << std::endl;
}

void removeFragrance(sqlite3* p_db)
{
    int rc;
    char* errorMsg = 0;

    size_t choice = 0;

    /* Input Data */
    std::cout << "|-----REMOVE FRAGRANCE-----|\nSelect fragrance to remove (id): ";
    std::cin >> choice;
    std::cout << std::endl;

    std::string sql = "DELETE FROM fragrances WHERE id=" + std::to_string(choice) + ";";
    rc = sqlite3_exec(p_db, sql.c_str(), NULL, 0, &errorMsg);

    if (rc != SQLITE_OK)  
    
    {
        std::cout << "SQL error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    }
    else
        std::cout << "Fragrance removed from database!\n";
}

void displayFavorites(sqlite3* p_db)
{
    int rc;
    char* errorMsg = 0;
    size_t choice;

    /* Get Favorite IDs */
    std::cout << "|-----FAVORITE FRAGRANCES-----|" << std::endl;
    std::string sql = "SELECT * FROM user" + std::to_string(user.id) + ";";
    rc = sqlite3_exec(p_db, sql.c_str(), callbackFavorites, 0, &errorMsg);

    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    } 

    if (favorites.size() == 0)
    {
        std::cout << "Favorites list is empty!" << std::endl;
        return;
    }

    /* Get Favorite's Info */
    sql = "SELECT * FROM 'fragrances' WHERE id=" + std::to_string(favorites[0]);
    for (size_t i = 1; i < favorites.size(); i++)
        sql += " OR id=" + std::to_string(favorites[i]);
    sql += ";";

    rc = sqlite3_exec(p_db, sql.c_str(), callbackFragrances, 0, &errorMsg);

    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    } 

    /* Display Favorite Fragrances */
    for (size_t i = 0; i < fragrances.size(); i++)
        std::cout << std::endl <<
        "id:" << fragrances[i].id << std::endl << 
        "name:" << fragrances[i].name << std::endl << 
        "fragrance:" << fragrances[i].fragrance << std::endl << 
        "strength:" << fragrances[i].strength << std::endl <<
        "price:" << fragrances[i].price << std::endl << std::endl;
}

void addFavorite(sqlite3* p_db)
{
    int rc;
    char* errorMsg = 0;
    size_t choice;

    std::cout << "|-----ADD FAVORITE FRAGRANCE-----|" << std::endl;

    /* Create Users Table */
    std::string sql = "CREATE TABLE IF NOT EXISTS user" + std::to_string(user.id) + " (favorite INTEGER NOT NULL);";
    rc = sqlite3_exec(p_db, sql.c_str(), NULL, 0, &errorMsg);

    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    } 

    searchFragrances(p_db);
    
    /* Select Fragrances */
    std::cout << "Select fragrance to add to favorites (id): ";
    std::cin >> choice;
    std::cout << std::endl;

    sql = "INSERT INTO user" + std::to_string(user.id) + " (favorite) VALUES (" + std::to_string(choice) + ");";
    rc = sqlite3_exec(p_db, sql.c_str(), NULL, 0, &errorMsg);

    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    }
    else
        std::cout << "New fragrance added to favorites list!\n";
}

void removeFavorite (sqlite3* p_db)
{
    int rc;
    char* errorMsg = 0;
    size_t choice;

    displayFavorites(p_db);

    std::cout << "|-----REMOVE FAVORITE FRAGRANCE-----|\nSelect fragrance to remove from favorites (id): ";
    std::cin >> choice;
    std::cout << std::endl;

    std::string sql = "DELETE FROM user" + std::to_string(user.id) + " WHERE id=" + std::to_string(choice) + ";";
    rc = sqlite3_exec(p_db, sql.c_str(), NULL, 0, &errorMsg);

    if (rc != SQLITE_OK)
    {
        std::cout << "SQL error: " << errorMsg << std::endl;
        sqlite3_free(errorMsg);
    }
    else
        std::cout << "Fragrance removed from favorites list!\n";
}

int main()
{
    sqlite3* db;
    int rc;
    size_t choice = 0;

    /* Open Database */
    rc = sqlite3_open("fragrances.db", &db);
    
    if (rc)
    {
        std::cout << "Failed to open database:\n" << sqlite3_errmsg(db) << std::endl;
        return(0);
    } 
    else 
        std::cout << "Opened database successfully" << std::endl;

    
    /* Create Account / Login to Existing Account */
    std::cout << "|-----Welome to the Pamplemousse Fragrances Portal-----|" << std::endl;

    do{
        std::cout << "1.Create new account\n2.Login to existing account\nChoose: ";
        std::cin >> choice;
        std::cin.ignore(); //Clear for getline
        std::cout << std::endl;
        
        switch(choice)
        {
            case 1:
                createLogin(db);
                break;

            case 2:
                while (true)
                    if (checkLogin(db))
                        break;
                break;

            default:
                choice = 0;
                break;
        }
    }while (choice == 0);

    
    /* Main Menu */
    do{
        std::cout << std::endl << "1. Add Fragrance\n2. Remove Fragrance\n3. Search Fragrances\n4. Add Favorites\n5. Remove Favorites\n6. Display Favorites\n7. Exit\nChoose: ";

        std::cin >> choice;
        std::cin.ignore(); //Clear for getline

        std::cout << std::endl;

        switch(choice)
        {
            case 1:
                addFragrance(db);
                choice = 0;
                break;
            
            case 2:
                removeFragrance(db);
                choice = 0;
                break;

            case 3:
                searchFragrances(db);
                choice = 0;
                break;
            
            case 4:
                addFavorite(db);
                choice = 0;
                break;

            case 5:
                removeFavorite(db);
                choice = 0;
                break;

            case 6:
                displayFavorites(db);
                choice = 0;
                break;
            
            case 7:
                break;

            default:
                choice = 0;
                break;       
        }        
    }while (choice == 0);

    sqlite3_close(db);    
    return 0;
}
