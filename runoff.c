#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// Max voters and candidates
#define MAX_VOTERS 100
#define MAX_CANDIDATES 9

// preferences[i][j] is jth preference for voter i
int preferences[MAX_VOTERS][MAX_CANDIDATES];

// Candidates have name, vote count, eliminated status
typedef struct
{
    string name;
    int votes;
    bool eliminated;
}
candidate;

// Array of candidates
candidate candidates[MAX_CANDIDATES];

// Numbers of voters and candidates
int voter_count;
int candidate_count;

// Function prototypes
bool vote(int voter, int rank, string name);
void tabulate(void);
bool print_winner(void);
int find_min(void);
bool is_tie(int min);
void eliminate(int min);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: runoff [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX_CANDIDATES)
    {
        printf("Maximum number of candidates is %i\n", MAX_CANDIDATES);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i].name = argv[i + 1];
        candidates[i].votes = 0;
        candidates[i].eliminated = false;
    }

    voter_count = get_int("Number of voters: ");
    if (voter_count > MAX_VOTERS)
    {
        printf("Maximum number of voters is %i\n", MAX_VOTERS);
        return 3;
    }

    // Keep querying for votes
    for (int i = 0; i < voter_count; i++)
    {

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            // Record vote, unless it's invalid
            if (!vote(i, j, name))
            {
                printf("Invalid vote.\n");
                return 4;
            }
        }

        printf("\n");
    }

    // Keep holding runoffs until winner exists
    while (true)
    {
        // Calculate votes given remaining candidates
        tabulate();

        // Check if election has been won
        bool won = print_winner();
        if (won)
        {
            break;
        }

        // Eliminate last-place candidates
        int min = find_min();
        bool tie = is_tie(min);

        // If tie, everyone wins
        if (tie)
        {
            for (int i = 0; i < candidate_count; i++)
            {
                if (!candidates[i].eliminated)
                {
                    printf("%s\n", candidates[i].name);
                }
            }
            break;
        }

        // Eliminate anyone with minimum number of votes
        eliminate(min);

        // Reset vote counts back to zero
        for (int i = 0; i < candidate_count; i++)
        {
            candidates[i].votes = 0;
        }
    }
    return 0;
}

// TODO - https://youtu.be/-Vc5aGywKxo?t=449
// Record preference if vote is valid
bool vote(int voter, int rank, string name)
{
    // Check if voter choice is valid
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i].name) == 0)
        {
            preferences[voter][rank] = i;
            //printf("Voter: [%i], Rank: [%i], Candidate: [%s](%i) \n", voter, rank, name, i); // debug
            return true;
        }
    }
    return false; // catch all
}

// TODO - https://youtu.be/-Vc5aGywKxo?t=575
// For every voter, check thier preferences. If thier preferences has not been eliminated, add a vote to the candidate.
void tabulate(void)
{
    // itterate through all the voters
    for (int i = 0; i < voter_count; i++)
    {

        // itterate though the voter preferences
        for (int pref = 0; pref < 3; pref++)
        {

            // check if the voters preference has NOT been eliminated yet
            if (!candidates[preferences[i][pref]].eliminated)
            {
                // add 1 vote to candidate
                candidates[preferences[i][pref]].votes++;
                break; // break away from the voter pref for-loop (continue to the next voter)
            }
        }
    }

    return;
}

// Print the winner of the election, if there is one
// TODO - https://youtu.be/-Vc5aGywKxo?t=707
bool print_winner(void)
{
    int highest_vote = 0;

    // itterate through the candidates
    for (int i = 0; i < candidate_count; i++)
    {
        // find the highest vote count
        if (candidates[i].votes > highest_vote)
        {
            highest_vote = candidates[i].votes;
        }
    }

    // if the highest_vote count is greater than half of the total vote, return true
    if (highest_vote > (float) voter_count / 2)
    {

        // check each candidate
        for (int i = 0; i < candidate_count; i++)
        {
            // print winners
            if (candidates[i].votes == highest_vote)
            {
                printf("%s\n", candidates[i].name);
            }
        }
        return true;
    }

    return false;
}

// Return the minimum number of votes any remaining candidate has
// TODO - https://youtu.be/-Vc5aGywKxo?t=778
int find_min(void)
{
    int min_vote = MAX_VOTERS;

    // itterate through the candidates
    for (int i = 0; i < candidate_count; i++)
    {
        // find the lowest vote of the candidates that have not been eliminated yet
        if ((candidates[i].eliminated == false) && (candidates[i].votes < min_vote))
        {
            // set candidate vote count as the new min_vote
            min_vote = candidates[i].votes;
        }
    }

    return min_vote;
}

// Return true if the election is tied between all candidates, false otherwise
// TODO - https://youtu.be/-Vc5aGywKxo?t=858
bool is_tie(int min)
{
    int candidates_still_running = 0;
    int candidates_with_minvote_running = 0;

    // itterate through each candidates
    for (int i = 0; i < candidate_count; i++)
    {
        // keep track of candidates not yet eliminated
        if (candidates[i].eliminated == false)
        {
            candidates_still_running++;
        }

        // keep track of candidates not eliminated with min votes
        if ((candidates[i].eliminated == false) && (candidates[i].votes == round(min)))
        {
            candidates_with_minvote_running++;
        }

    }

    // check if the amount of running candidates with min vote is equal to the amount of candidates still running
    if (candidates_still_running == candidates_with_minvote_running)
    {
        return true; // detected a tie
    }

    return false; // did not detect a tie
}

// Eliminate the candidate (or candidates) in last place
// TODO - https://youtu.be/-Vc5aGywKxo?t=920
void eliminate(int min)
{
    // itterate through the candidates
    for (int i = 0; i < candidate_count; i++)
    {
        // check if the candidate has the min vote
        if (candidates[i].votes == min)
        {
            // eliminate the candidate
            candidates[i].eliminated = true;
        }
    }

    return;
}