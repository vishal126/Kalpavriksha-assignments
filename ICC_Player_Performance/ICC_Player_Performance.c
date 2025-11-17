#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "./Players_data.h"

#define NUMBEROFTEAMS 10
#define MINID 1
#define MAXID 1500
#define MINPLAYERS 11
#define MAXPLAYERS 50
#define MAXNAMELENGTH 50

enum Role
{
    BATSMAN = 1,
    BOWLER = 2,
    ALLROUNDER = 3
};

typedef struct playerObject
{
    unsigned short playerId;
    char name[MAXNAMELENGTH + 1];
    char teamName[MAXNAMELENGTH + 1];
    char role[MAXNAMELENGTH + 1];
    unsigned int totalRuns;
    float battingAverage;
    float strikeRate;
    unsigned int wickets;
    float economyRate;
    float performanceIndex;
    struct playerObject *next;
} playerObject;

typedef struct roleNode
{
    struct playerObject *player;
    struct roleNode *next;
} roleNode;

typedef struct teamObject
{
    unsigned short teamId;
    char name[MAXNAMELENGTH + 1];
    unsigned short totalPlayers;
    float averageBattingStrikeRate;
    unsigned short battingPlayers;
    struct teamObject *next;
    struct playerObject *playersStart;
    roleNode *batsmanStart;
    roleNode *bowlerStart;
    roleNode *allRounderStart;
} teamObject;

typedef struct playerData
{
    unsigned short teamId;
    unsigned short playerId;
    char playerName[MAXNAMELENGTH + 1];
    enum Role playerRole;
    unsigned int playerTotalRuns;
    float playerBattingAverage;
    float playerStrikeRate;
    unsigned int playerWickets;
    float playerEconomyRate;
} playerData;

teamObject *teamsHead = NULL;
teamObject *teamsTail = NULL;

teamObject *teamPointer[NUMBEROFTEAMS];

unsigned int getIntInput(const char *prompt)
{
    char buffer[100];
    unsigned int value;
    while (1)
    {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            buffer[strcspn(buffer, "\n")] = 0;

            if (sscanf(buffer, "%u", &value) == 1)
            {
                return value;
            }
            else
            {
                printf("Invalid input. Please enter a valid integer.\n");
            }
        }
        else
        {
            printf("Error reading input. Try again.\n");
        }
    }

    return 0;
}

float getFloatInput(const char *prompt)
{
    char buffer[100];
    float value;

    while (1)
    {
        printf("%s", prompt);

        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            if (sscanf(buffer, "%f", &value) == 1)
            {
                return value;
            }
            else
            {
                printf("Invalid input. Please enter a valid Floating number.\n");
            }
        }
        else
        {
            printf("Error reading input. Try again.\n");
        }
    }

    return 0.0;
}

enum Role getEnumRole(const char *role)
{
    if (stricmp(role, "batsman") == 0)
    {
        return 1;
    }
    else if (stricmp(role, "bowler") == 0)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

float getPerformanceIndex(const char *role, const float battingAverage, const float strikeRate, const unsigned int wickets, const float economyRate)
{
    float performanceIndex;
    if (getEnumRole(role) == BATSMAN)
    {
        performanceIndex = (battingAverage * strikeRate) / 100;
    }
    else if (getEnumRole(role) == BOWLER)
    {
        performanceIndex = (wickets * 2) + (100 - economyRate);
    }
    else
    {
        performanceIndex = ((battingAverage * strikeRate) / 100) + (wickets * 2);
    }

    return performanceIndex;
}

void freePlayers()
{
    teamObject *teamCounter = teamsHead;

    while (teamCounter)
    {
        playerObject *playerCounter = teamCounter->playersStart;
        while (playerCounter)
        {
            playerObject *nextPlayer = playerCounter->next;
            free(playerCounter);
            playerCounter = nextPlayer;
        }

        roleNode *batsmanCounter = teamCounter->batsmanStart;
        while (batsmanCounter)
        {
            roleNode *nextBatsman = batsmanCounter->next;
            free(batsmanCounter);
            batsmanCounter = nextBatsman;
        }
        roleNode *bowlerCounter = teamCounter->bowlerStart;
        while (bowlerCounter)
        {
            roleNode *nextBowler = bowlerCounter->next;
            free(bowlerCounter);
            bowlerCounter = nextBowler;
        }
        roleNode *allRounderCounter = teamCounter->allRounderStart;
        while (allRounderCounter)
        {
            roleNode *nextAllRounder = allRounderCounter->next;
            free(allRounderCounter);
            allRounderCounter = nextAllRounder;
        }
        teamCounter = teamCounter->next;
    }
}

void freeTeams()
{
    freePlayers();

    teamObject *teamCounter = teamsHead;

    while (teamCounter)
    {
        teamObject *nextTeam = teamCounter->next;
        free(teamCounter);
        teamCounter = nextTeam;
    }

    teamsHead = NULL;
    teamsTail = NULL;
}

void initializeTeams(int *error)
{
    for (int i = 0; i < teamCount; i++)
    {
        teamObject *team = malloc(sizeof(teamObject));
        if (!team)
        {
            freeTeams();
            printf("Memory allocation failed!\n");
            *error = -1;
            return;
        }

        strcpy(team->name, teams[i]);
        team->teamId = i + 1;
        team->totalPlayers = 0;
        team->battingPlayers = 0;
        team->averageBattingStrikeRate = 0;

        team->playersStart = NULL;

        team->batsmanStart = NULL;
        team->bowlerStart = NULL;
        team->allRounderStart = NULL;

        team->next = NULL;

        if (teamsHead == NULL)
            teamsHead = team;
        else
            teamsTail->next = team;

        teamsTail = team;
        teamPointer[i] = team;
    }

    *error = 0;
}

void insertIntoRoleList(roleNode **start, playerObject *player, int *error)
{
    roleNode *newNode = malloc(sizeof(roleNode));
    if (!newNode)
    {
        freePlayers();
        printf("Memory allocation failed!\n");
        *error = -1;
        return;
    }
    newNode->player = player;
    newNode->next = NULL;

    roleNode *current = *start;
    roleNode *previous = NULL;

    if (current == NULL)
    {
        *start = newNode;
        return;
    }

    while (current != NULL && current->player->performanceIndex > player->performanceIndex)
    {
        previous = current;
        current = current->next;
    }

    if (previous == NULL)
    {
        newNode->next = *start;
        *start = newNode;
        return;
    }

    if (current != NULL)
    {
        previous->next = newNode;
        newNode->next = current;
        return;
    }

    previous->next = newNode;
}

void InsertNodeSorted(teamObject *team, playerObject *node)
{
    playerObject *cur = team->playersStart;
    playerObject *prev = NULL;

    if (cur == NULL)
    {
        team->playersStart = node;
    }
    else
    {
        while (cur && cur->performanceIndex > node->performanceIndex)
        {
            prev = cur;
            cur = cur->next;
        }

        if (prev == NULL)
        {
            node->next = team->playersStart;
            team->playersStart = node;
        }
        else if (cur != NULL)
        {
            prev->next = node;
            node->next = cur;
        }
        else
        {
            prev->next = node;
        }
    }
}

void checkAndInsertInRole(playerObject *node, teamObject *team, int *error)
{
    if (getEnumRole(node->role) == BATSMAN)
    {
        insertIntoRoleList(&team->batsmanStart, node, error);
        if (*error != 0)
        {
            return;
        }
    }
    else if (getEnumRole(node->role) == BOWLER)
    {
        insertIntoRoleList(&team->bowlerStart, node, error);
        if (*error != 0)
        {
            return;
        }
    }
    else
    {
        insertIntoRoleList(&team->allRounderStart, node, error);
        if (*error != 0)
        {
            return;
        }
    }
}

void initializePlayers(int *error)
{
    teamObject *team = teamsHead;

    while (team != NULL)
    {
        float totalStrikeRate = 0;
        unsigned short battingPlayers = 0;
        unsigned short totalPlayers = 0;

        for (unsigned short playerCounter = 0; playerCounter < playerCount; playerCounter++)
        {
            Player p = players[playerCounter];

            if (stricmp(p.team, team->name) != 0)
                continue;

            playerObject *node = malloc(sizeof(playerObject));
            if (!node)
            {
                *error = -1;
                freePlayers();
                printf("Memory allocation failed!\n");
                return;
            }

            node->playerId = (unsigned short)p.id;
            strcpy(node->name, p.name);
            strcpy(node->teamName, p.team);
            strcpy(node->role, p.role);
            node->totalRuns = (unsigned int)p.totalRuns;
            node->battingAverage = p.battingAverage;
            node->strikeRate = p.strikeRate;
            node->wickets = (unsigned int)p.wickets;
            node->economyRate = p.economyRate;
            node->performanceIndex = getPerformanceIndex(
                node->role,
                node->battingAverage,
                node->strikeRate,
                node->wickets,
                node->economyRate);

            node->next = NULL;

            if (getEnumRole(node->role) == BATSMAN ||
                getEnumRole(node->role) == ALLROUNDER)
            {
                totalStrikeRate += node->strikeRate;
                battingPlayers++;
            }

            totalPlayers++;

            InsertNodeSorted(team, node);

            checkAndInsertInRole(node, team, error);

            if (*error != 0)
            {
                return;
            }
        }

        team->totalPlayers = totalPlayers;
        team->battingPlayers = battingPlayers;

        if (battingPlayers > 0)
            team->averageBattingStrikeRate = totalStrikeRate / battingPlayers;
        else
            team->averageBattingStrikeRate = 0;

        team = team->next;
    }

    *error = 0;
}

teamObject *findTeam(const unsigned short teamId)
{
    unsigned short low = 0;
    unsigned short high = teamCount - 1;

    while (low <= high)
    {
        unsigned short mid = (low + high) / 2;
        unsigned short midId = teamPointer[mid]->teamId;

        if (teamId == midId)
            return teamPointer[mid];
        if (teamId > midId)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return NULL;
}

void checkForDuplicatePlayerId(const unsigned short playerId, const teamObject *team, int *error)
{
    *error = 0;
    playerObject *playerCounter = team->playersStart;

    while (playerCounter != NULL)
    {
        if (playerCounter->playerId == playerId)
        {
            *error = -1;
            return;
        }

        playerCounter = playerCounter->next;
    }
}

void addPlayerToTeam(playerData player, int *error)
{
    *error = 0;
    teamObject *team = findTeam(player.teamId);
    if (!team)
    {
        printf("Team not found\n");
        return;
    }

    if (team->totalPlayers >= MAXPLAYERS)
    {
        printf("Team has already 50 players!\n");
        return;
    }

    checkForDuplicatePlayerId(player.playerId, team, error);
    if (*error != 0)
    {
        printf("Player with id %hu exists in team %s\n", player.playerId, team->name);
        return;
    }

    playerObject *tempPlayer = malloc(sizeof(playerObject));

    if (!tempPlayer)
    {
        freePlayers();
        printf("Memory allocation failed!\n");
        *error = -1;
        return;
    }

    tempPlayer->playerId = player.playerId;
    strcpy(tempPlayer->name, player.playerName);
    strcpy(tempPlayer->teamName, team->name);

    if (player.playerRole == BATSMAN)
        strcpy(tempPlayer->role, "Batsman");
    else if (player.playerRole == BOWLER)
        strcpy(tempPlayer->role, "Bowler");
    else
        strcpy(tempPlayer->role, "All-rounder");

    tempPlayer->totalRuns = player.playerTotalRuns;
    tempPlayer->battingAverage = player.playerBattingAverage;
    tempPlayer->strikeRate = player.playerStrikeRate;
    tempPlayer->wickets = player.playerWickets;
    tempPlayer->economyRate = player.playerEconomyRate;

    tempPlayer->performanceIndex = getPerformanceIndex(tempPlayer->role,
                                                       tempPlayer->battingAverage,
                                                       tempPlayer->strikeRate,
                                                       tempPlayer->wickets,
                                                       tempPlayer->economyRate);

    tempPlayer->next = NULL;

    InsertNodeSorted(team, tempPlayer);

    checkAndInsertInRole(tempPlayer, team, error);

    if (*error != 0)
    {
        return;
    }

    if (getEnumRole(tempPlayer->role) == BATSMAN || getEnumRole(tempPlayer->role) == ALLROUNDER)
    {
        team->averageBattingStrikeRate = (((team->averageBattingStrikeRate * team->battingPlayers) + (tempPlayer->strikeRate)) / (team->battingPlayers + 1));
        team->battingPlayers++;
    }

    team->totalPlayers++;

    printf("Player added to team %s successfully\n", team->name);
}

unsigned short getId(char *prompt)
{
    unsigned short id = getIntInput(prompt);

    while (id < MINID || id > MAXID)
    {
        printf("ID is not in range(1-1500), enter again!\n");
        id = getIntInput(prompt);
    }

    return (unsigned short)id;
}

void getPlayerName(char *playerName)
{
    while (1)
    {
        printf("Name: ");
        if (fgets(playerName, MAXNAMELENGTH + 1, stdin) != NULL)
        {
            playerName[strcspn(playerName, "\n")] = 0;
            return;
        }
        else
        {
            printf("Error while getting name, enter again.\n");
        }
    }
}

enum Role getPlayerRole(const char *prompt)
{
    unsigned short role = getIntInput(prompt);

    while (role < 1 || role > 3)
    {
        printf("Invalid role, enter again!\n");
        role = (unsigned short)getIntInput(prompt);
    }

    return role;
}

void addPlayer()
{
    playerData player;

    player.teamId = getId("Enter Team ID to add player: ");
    printf("Enter Player Details: \n");
    player.playerId = getId("Player ID: ");
    getPlayerName(player.playerName);
    player.playerRole = getPlayerRole("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    player.playerTotalRuns = getIntInput("Total Runs: ");
    player.playerBattingAverage = getFloatInput("Batting Average: ");
    player.playerStrikeRate = getFloatInput("Strike Rate: ");
    player.playerWickets = getIntInput("Wickets: ");
    player.playerEconomyRate = getFloatInput("Ecomomy Rate: ");

    int error = 0;

    addPlayerToTeam(player, &error);
    if (error != 0)
    {
        return;
    }
}

void displayPlayersOfSpecificTeam()
{
    unsigned short teamId = getId("Enter Team ID: ");

    teamObject *team = findTeam(teamId);

    if (team == NULL)
    {
        printf("No team found with given Id!\n");
        return;
    }

    if (team->playersStart == NULL)
    {
        printf("No player exists in team: %s!\n", team->name);
        return;
    }

    playerObject *playerCounter = team->playersStart;

    printf("Players of Team %s:\n", team->name);
    printf("===============================================================================================================================\n");
    printf("%-5s %-50s %-12s %-6s %-8s %-8s %-6s %-6s %-10s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("===============================================================================================================================\n");
    while (playerCounter != NULL)
    {
        printf("%-5hu %-50s %-12s %-6u %-8.2f %-8.2f %-6u %-6.2f %-10.2f\n",
               playerCounter->playerId,
               playerCounter->name,
               playerCounter->role,
               playerCounter->totalRuns,
               playerCounter->battingAverage,
               playerCounter->strikeRate,
               playerCounter->wickets,
               playerCounter->economyRate,
               playerCounter->performanceIndex);

        playerCounter = playerCounter->next;
    }

    printf("===============================================================================================================================\n");

    printf("Total Players: %hu\n", team->totalPlayers);
    printf("Average Batting Strike Rate: %f\n", team->averageBattingStrikeRate);
    printf("\n");
}

teamObject *getMiddleTeam(teamObject *head)
{
    teamObject *slow = head;
    teamObject *fast = head->next;

    while (fast != NULL && fast->next != NULL)
    {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;
}

teamObject *mergeTeams(teamObject *list1, teamObject *list2)
{
    teamObject dummy;

    dummy.next = NULL;

    teamObject *temp = &dummy;

    while (list1 != NULL && list2 != NULL)
    {
        if (list1->averageBattingStrikeRate > list2->averageBattingStrikeRate)
        {
            temp->next = list1;
            list1 = list1->next;
        }
        else
        {
            temp->next = list2;
            list2 = list2->next;
        }
        temp = temp->next;
    }

    if (list1)
        temp->next = list1;
    else
        temp->next = list2;

    return dummy.next;
}

teamObject *mergeSortTeams(teamObject *head)
{
    if (head == NULL || head->next == NULL)
    {
        return head;
    }

    teamObject *middle = getMiddleTeam(head);

    teamObject *leftHead = head;
    teamObject *rightHead = middle->next;
    middle->next = NULL;

    leftHead = mergeSortTeams(leftHead);
    rightHead = mergeSortTeams(rightHead);

    return mergeTeams(leftHead, rightHead);
}

teamObject *updateTeamTail(teamObject *head)
{
    if (head == NULL)
        return NULL;

    while (head->next != NULL)
        head = head->next;

    return head;
}

void displayTeamsByAverageBattingStrikeRate()
{
    teamsHead = mergeSortTeams(teamsHead);
    teamsTail = updateTeamTail(teamsHead);

    teamObject *teamCounter = teamsHead;

    printf("Teams Sorted by Average Batting Strike Rate\n");
    printf("===============================================================================================================================\n");
    printf("%-5s %-50s %-8s %-6s\n", "ID", "Team Name", "Avg Bat SR", "Total Players");
    printf("===============================================================================================================================\n");

    while (teamCounter != NULL)
    {
        printf("%-5hu\t%-50s\t%-8.2f\t%-6hu\n", teamCounter->teamId, teamCounter->name, teamCounter->averageBattingStrikeRate, teamCounter->totalPlayers);
        teamCounter = teamCounter->next;
    }
    printf("===============================================================================================================================\n");
}

void checkForKPlayers(roleNode *rolePlayers, unsigned short K, int *error)
{
    *error = 0;
    unsigned short count = 0;
    while (rolePlayers)
    {
        count++;
        rolePlayers = rolePlayers->next;
    }

    if (count < K)
    {
        *error = -1;
    }
}

void displayTopKOfSpecificTeam()
{
    unsigned short teamId = getId("Enter Team ID: ");
    enum Role playerRole = getPlayerRole("Role (1 BATSMAN, 2 BOWLER 3 ALL-ROUNDER):");
    unsigned short K = (unsigned short)getIntInput("K: ");

    teamObject *team = findTeam(teamId);
    if (!team)
    {
        printf("Team not found\n");
        return;
    }

    roleNode *rolePlayers;

    if (playerRole == BATSMAN)
    {
        printf("Top K Batsmen:\n");
        rolePlayers = team->batsmanStart;
    }
    else if (playerRole == BOWLER)
    {
        printf("Top K Bowlers:\n");
        rolePlayers = team->bowlerStart;
    }
    else
    {
        printf("Top K All-rounders:\n");
        rolePlayers = team->allRounderStart;
    }

    int error = 0;

    checkForKPlayers(rolePlayers, K, &error);

    if (error != 0)
    {
        printf("K %hu is greater than the total batsman player in team\n", K);
        return;
    }

    printf("===============================================================================================================================\n");
    printf("%-5s %-50s %-12s %-8s %-8s %-8s %-8s %-8s %-8s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf. Index");
    printf("===============================================================================================================================\n");

    unsigned short count = 0;
    while (rolePlayers != NULL && count < K)
    {
        playerObject *player = rolePlayers->player;
        printf("%-5hu %-50s %-12s %-8u %-8.2f %-8.2f %-8u %-8.2f %-8.2f\n",
               player->playerId,
               player->name,
               player->role,
               player->totalRuns,
               player->battingAverage,
               player->strikeRate,
               player->wickets,
               player->economyRate,
               player->performanceIndex);

        rolePlayers = rolePlayers->next;
        count++;
    }
    printf("===============================================================================================================================\n");
}

roleNode *cloneList(roleNode *head, int *error)
{
    *error = 0;
    roleNode *newHead = NULL;
    roleNode *newTail = NULL;

    while (head)
    {
        roleNode *newNode = malloc(sizeof(roleNode));
        if (!newNode)
        {
            printf("Memory allocation failed while cloning list.\n");
            *error = -1;

            roleNode *tmp = newHead;
            while (tmp)
            {
                roleNode *n = tmp->next;
                free(tmp);
                tmp = n;
            }
            return NULL;
        }
        newNode->player = head->player;
        newNode->next = NULL;

        if (!newHead)
            newHead = newTail = newNode;
        else
        {
            newTail->next = newNode;
            newTail = newNode;
        }

        head = head->next;
    }
    return newHead;
}

roleNode *mergeTwoRoleLists(roleNode *A, roleNode *B)
{
    roleNode dummy;
    roleNode *tail = &dummy;
    dummy.next = NULL;

    while (A && B)
    {
        if (A->player->performanceIndex >= B->player->performanceIndex)
        {
            tail->next = A;
            A = A->next;
        }
        else
        {
            tail->next = B;
            B = B->next;
        }
        tail = tail->next;
    }

    tail->next = (A ? A : B);
    return dummy.next;
}

roleNode *mergeKRoleLists(roleNode *lists[], int t)
{
    if (t == 0)
        return NULL;

    while (t > 1)
    {
        int i = 0;
        int j = t - 1;

        while (i < j)
        {
            lists[i] = mergeTwoRoleLists(lists[i], lists[j]);
            i++;
            j--;
        }

        t = (t + 1) / 2;
    }

    return lists[0];
}

void displayAllPlayersOfSpecificRole()
{
    enum Role playerRole = getPlayerRole("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");

    roleNode *tempLists[NUMBEROFTEAMS];
    int t = 0;
    teamObject *team = teamsHead;

    while (team)
    {
        int err = 0;
        roleNode *cloned = NULL;
        if (playerRole == BATSMAN)
            cloned = cloneList(team->batsmanStart, &err);
        else if (playerRole == BOWLER)
            cloned = cloneList(team->bowlerStart, &err);
        else
            cloned = cloneList(team->allRounderStart, &err);

        if (err != 0)
        {
            for (int k = 0; k < t; ++k)
            {
                roleNode *r = tempLists[k];
                while (r)
                {
                    roleNode *n = r->next;
                    free(r);
                    r = n;
                }
            }
            return;
        }

        tempLists[t++] = cloned;
        team = team->next;
    }

    roleNode *merged = mergeKRoleLists(tempLists, t);

    printf("All players for this role across all teams:\n");
    printf("===============================================================================================================================\n");
    printf("%-5s %-50s %-12s %-8s %-8s %-8s %-8s %-8s %-8s\n", "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf. Index");
    printf("===============================================================================================================================\n");

    roleNode *playerMerged = merged;
    while (playerMerged)
    {
        playerObject *player = playerMerged->player;
        printf("%-5hu %-50s %-12s %-8u %-8.2f %-8.2f %-8u %-8.2f %-8.2f\n",
               player->playerId,
               player->name,
               player->role,
               player->totalRuns,
               player->battingAverage,
               player->strikeRate,
               player->wickets,
               player->economyRate,
               player->performanceIndex);
        playerMerged = playerMerged->next;
    }
    printf("===============================================================================================================================\n");

    playerMerged = merged;
    while (playerMerged)
    {
        roleNode *next = playerMerged->next;
        free(playerMerged);
        playerMerged = next;
    }
}

int main()
{
    int error = 0;
    initializeTeams(&error);
    if (error != 0)
    {
        freeTeams();
        return -1;
    }

    error = 0;
    initializePlayers(&error);
    if (error == -1)
    {
        freeTeams();
        return -1;
    }

    while (1)
    {
        printf("==============================================================================\n");
        printf("ICC ODI Player Performance Analyzer\n");
        printf("==============================================================================\n");
        printf("1. Add Player to Team\n2. Display Players of a Specific Team\n3. Display Teams by Average Batting Strike Rate\n4. Display Top K Players of a Specific Team by Role\n5. Display all Players of specific role Across All Teams by performance index\n6. Exit\n");
        printf("==============================================================================\n");

        short int choice = getIntInput("Enter your choice: ");

        switch (choice)
        {
        case 1:
            addPlayer();
            break;

        case 2:
            displayPlayersOfSpecificTeam();
            break;

        case 3:
            displayTeamsByAverageBattingStrikeRate();
            break;

        case 4:
            displayTopKOfSpecificTeam();
            break;

        case 5:
            displayAllPlayersOfSpecificRole();
            break;

        case 6:
            printf("Releasing memory.\n");
            freeTeams();
            exit(0);

        default:
            printf("enter valid choice!\n");
            break;
        }
    }
}