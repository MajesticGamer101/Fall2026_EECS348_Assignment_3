/*
 * Program name: EECS 348 Assignment - CEO Email Priority Queue (GenAI version)
 *
 * Description:
 *   C++ program that prioritizes the emails in a busy CEO's Inbox.
 *   Emails are stored in a priority queue that is implemented from scratch
 *   as a list-based (array-based) MaxHeap. Priority is decided first by the
 *   sender category (Boss > Subordinate > Peer > ImportantPerson >
 *   OtherPerson) and then by date, with the NEWEST email read first.
 *   The program is built from three classes:
 *     Email   - one email and the rule for comparing two emails
 *     MaxHeap - list-based max heap of Email objects
 *     Inbox   - reads the test file and carries out each command
 *
 * Inputs:
 *   A text test file containing one command per line:
 *     EMAIL <sender category>,<subject line>,<date as MM-DD-YYYY>
 *     NEXT
 *     READ
 *     COUNT
 *   The file name may be given as the first command-line argument.
 *   If no argument is given, the program asks the user for the file name.
 *
 * Output:
 *   Terminal output:
 *     NEXT  - displays the sender, subject and date of the highest
 *             priority email (the email stays in the queue)
 *     READ  - removes the highest priority email (no display)
 *     COUNT - displays the number of unread emails
 *
 * Collaborators: None
 *
 * Other sources for the code: Claude (Anthropic) generative AI
 *
 * Author: Joseph Akins
 *
 * Creation date: 10-01-2026
 *
 * Revision date: 10-01-2026
 *
 * Revisions:
 *   10-01-2026 - Initial version generated with Claude
 *   10-01-2026 - swapItems now uses std::swap (moves instead of copies);
 *                added #include <utility>
 */

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

/*
 * Class: Email
 * Represents a single email and knows how to compare itself with another
 * email to decide which one the CEO should read first.
 */
class Email {
public:
    // Default constructor: builds an empty, lowest priority email.
    Email()
        : senderCategory(""), subject(""), date(""),
          categoryRank(0), dateKey(0), arrivalOrder(0) {}

    // Inputs: sender category, subject line, date (MM-DD-YYYY) and the
    //         order in which the email arrived in the Inbox.
    Email(const std::string& sender, const std::string& subjectLine,
          const std::string& dateText, long arrival)
        : senderCategory(sender), subject(subjectLine), date(dateText),
          categoryRank(rankOf(sender)), dateKey(dateKeyOf(dateText)),
          arrivalOrder(arrival) {}

    // Returns true when this email should be read before the other email.
    // Input: the email to compare against.
    bool hasHigherPriorityThan(const Email& other) const {
        // 1. A more important sender category always wins.
        if (categoryRank != other.categoryRank) {
            return categoryRank > other.categoryRank;
        }
        // 2. Same category: the newest date wins.
        if (dateKey != other.dateKey) {
            return dateKey > other.dateKey;
        }
        // 3. Same category and date: the email that arrived last is newest.
        return arrivalOrder > other.arrivalOrder;
    }

    // Output: prints the email to the terminal in the required format.
    void display() const {
        std::cout << "Sender: " << senderCategory << std::endl;
        std::cout << "Subject: " << subject << std::endl;
        std::cout << "Date: " << date << std::endl;
    }

private:
    std::string senderCategory; // One of the five sender categories
    std::string subject;        // Subject line (may contain spaces)
    std::string date;           // Date exactly as read: MM-DD-YYYY
    int categoryRank;           // Larger number = read sooner
    int dateKey;                // Date as YYYYMMDD so dates compare as numbers
    long arrivalOrder;          // Position of the email in the test file

    // Converts a sender category into a numeric rank.
    // Input: sender category string. Output: 5 (Boss) down to 1 (OtherPerson).
    static int rankOf(const std::string& sender) {
        if (sender == "Boss") return 5;
        if (sender == "Subordinate") return 4;
        if (sender == "Peer") return 3;
        if (sender == "ImportantPerson") return 2;
        if (sender == "OtherPerson") return 1;
        return 0; // Unknown category: lowest priority
    }

    // Converts MM-DD-YYYY into the integer YYYYMMDD.
    // Input: date string. Output: sortable integer (0 if the date is malformed).
    static int dateKeyOf(const std::string& dateText) {
        if (dateText.length() != 10 || dateText[2] != '-' || dateText[5] != '-') {
            return 0;
        }
        int key = 0;
        const int order[8] = {6, 7, 8, 9, 0, 1, 3, 4}; // YYYY, then MM, then DD
        for (int i = 0; i < 8; i++) {
            char digit = dateText[order[i]];
            if (digit < '0' || digit > '9') {
                return 0;
            }
            key = key * 10 + (digit - '0');
        }
        return key;
    }
};

/*
 * Class: MaxHeap
 * A max heap of Email objects stored in a list (std::vector). The email
 * with the highest priority is always at index 0. For the item at index i:
 *   parent = (i - 1) / 2, left child = 2i + 1, right child = 2i + 2
 * No pre-existing heap or priority queue modules are used.
 */
class MaxHeap {
public:
    // Adds an email to the heap. Input: the email to add.
    void insert(const Email& email) {
        items.push_back(email);       // Place at the end of the list
        siftUp(size() - 1);           // Move it up to its correct position
    }

    // Returns the highest priority email without removing it.
    // The caller must make sure the heap is not empty.
    const Email& peek() const {
        return items[0];
    }

    // Removes the highest priority email.
    // Output: true if an email was removed, false if the heap was empty.
    bool removeMax() {
        if (isEmpty()) {
            return false;
        }
        items[0] = items[size() - 1]; // Move the last item to the root
        items.pop_back();             // Shrink the list
        if (!isEmpty()) {
            siftDown(0);              // Move the new root down into place
        }
        return true;
    }

    // Output: number of emails currently in the heap.
    int size() const {
        return static_cast<int>(items.size());
    }

    // Output: true when the heap holds no emails.
    bool isEmpty() const {
        return items.empty();
    }

private:
    std::vector<Email> items; // The list that stores the heap

    // Index helpers
    int parentOf(int index) const { return (index - 1) / 2; }
    int leftOf(int index) const { return 2 * index + 1; }
    int rightOf(int index) const { return 2 * index + 2; }

    // Exchanges the emails stored at two indexes.
    void swapItems(int first, int second) {
        std::swap(items[first], items[second]);
    }

    // Moves the item at index up until its parent has higher priority.
    void siftUp(int index) {
        while (index > 0 &&
               items[index].hasHigherPriorityThan(items[parentOf(index)])) {
            swapItems(index, parentOf(index));
            index = parentOf(index);
        }
    }

    // Moves the item at index down until both children have lower priority.
    void siftDown(int index) {
        while (true) {
            int largest = index;
            int left = leftOf(index);
            int right = rightOf(index);

            if (left < size() &&
                items[left].hasHigherPriorityThan(items[largest])) {
                largest = left;
            }
            if (right < size() &&
                items[right].hasHigherPriorityThan(items[largest])) {
                largest = right;
            }
            if (largest == index) {
                return; // Heap property restored
            }
            swapItems(index, largest);
            index = largest;
        }
    }
};

/*
 * Class: Inbox
 * The CEO's Inbox. Reads the test file and uses a MaxHeap as a priority
 * queue to carry out the EMAIL, NEXT, READ and COUNT commands.
 */
class Inbox {
public:
    Inbox() : arrivalCounter(0) {}

    // Reads the test file and processes every command in it.
    // Input: name of the test file.
    // Output: true if the file was opened, false otherwise.
    bool processFile(const std::string& fileName) {
        std::ifstream file(fileName.c_str());
        if (!file.is_open()) {
            return false;
        }
        std::string line;
        while (std::getline(file, line)) {
            processCommand(trim(line));
        }
        file.close();
        return true;
    }

private:
    MaxHeap queue;       // Priority queue of unread emails
    long arrivalCounter; // Counts emails so ties can be broken by arrival

    // Decides which command a line holds and carries it out.
    // Input: one line of the test file with outer whitespace removed.
    void processCommand(const std::string& line) {
        if (line.empty()) {
            return; // Skip blank lines
        }
        if (line.compare(0, 6, "EMAIL ") == 0) {
            addEmail(line.substr(6));
        } else if (line == "NEXT") {
            showNext();
        } else if (line == "READ") {
            markRead();
        } else if (line == "COUNT") {
            showCount();
        } else {
            std::cout << "Unknown command: " << line << std::endl;
        }
    }

    // EMAIL command: builds an email and places it in the queue.
    // Input: the text after "EMAIL ", i.e. <category>,<subject>,<date>
    void addEmail(const std::string& details) {
        std::size_t firstComma = details.find(',');
        std::size_t lastComma = details.rfind(',');
        if (firstComma == std::string::npos || firstComma == lastComma) {
            std::cout << "Invalid EMAIL command: " << details << std::endl;
            return;
        }
        std::string sender = trim(details.substr(0, firstComma));
        std::string subject =
            trim(details.substr(firstComma + 1, lastComma - firstComma - 1));
        std::string date = trim(details.substr(lastComma + 1));

        arrivalCounter++;
        queue.insert(Email(sender, subject, date, arrivalCounter));
    }

    // NEXT command: displays the highest priority email, leaving it in the
    // queue so a second NEXT shows the same email.
    void showNext() const {
        if (queue.isEmpty()) {
            std::cout << "No emails to read." << std::endl;
            return;
        }
        std::cout << "Next email:" << std::endl;
        queue.peek().display();
    }

    // READ command: removes the highest priority email without displaying it.
    void markRead() {
        if (!queue.removeMax()) {
            std::cout << "No emails to read." << std::endl;
        }
    }

    // COUNT command: displays the number of unread emails.
    void showCount() const {
        std::cout << "There are " << queue.size() << " emails to read."
                  << std::endl;
    }

    // Removes spaces, tabs and line-ending characters from both ends of text.
    // Input: any string. Output: the trimmed string.
    static std::string trim(const std::string& text) {
        const std::string whitespace = " \t\r\n";
        std::size_t start = text.find_first_not_of(whitespace);
        if (start == std::string::npos) {
            return "";
        }
        std::size_t end = text.find_last_not_of(whitespace);
        return text.substr(start, end - start + 1);
    }
};

/*
 * main
 * Inputs: optional command-line argument holding the test file name.
 * Output: 0 on success, 1 if the test file could not be opened.
 */
int main(int argc, char* argv[]) {
    std::string fileName;
    if (argc > 1) {
        fileName = argv[1];
    } else {
        std::cout << "Enter the test file name: ";
        std::getline(std::cin, fileName);
    }

    Inbox inbox;
    if (!inbox.processFile(fileName)) {
        std::cout << "Error: could not open file " << fileName << std::endl;
        return 1;
    }
    return 0;
}