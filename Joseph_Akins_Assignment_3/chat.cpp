/*
 * Program: EECS 348 Assignment 2 - CEO Email Priority Queue
 *
 * Description:
 *   C++ program that prioritizes a CEO's unread emails using a MaxHeap.
 *   Emails are prioritized first by sender category and then by date.
 *   Newer emails from the same sender category are given higher priority.
 *
 * Input:
 *   Commands read from a test file or standard input:
 *
 *     EMAIL <sender category>,<subject line>,<date>
 *     NEXT
 *     READ
 *     COUNT
 *
 * Output:
 *   Displays the number of unread emails and the highest-priority email
 *   when requested.
 *
 * Collaborators: None
 * Other Sources: ChatGPT (OpenAI)
 * Author: Joseph Akins
 * Creation Date: October 1, 2026
 * Revision Date: October 1, 2026
 *
 * Revisions:
 *   Initial C++ object-oriented implementation using a custom MaxHeap.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

/*
 * Email object
 *
 * Stores all information associated with one email.
 */
class Email {
private:
    string sender;
    string subject;
    string date;

    int senderPriority;
    int dateValue;

    long long arrivalOrder;

public:

    /*
     * Constructor
     *
     * Creates an Email object and determines its sender priority
     * and numeric date value.
     */
    Email(const string& senderCategory,
          const string& subjectLine,
          const string& emailDate,
          long long order)
    {
        sender = senderCategory;
        subject = subjectLine;
        date = emailDate;
        arrivalOrder = order;

        /*
         * Higher numbers mean higher priority.
         */
        if (sender == "Boss") {
            senderPriority = 5;
        }
        else if (sender == "Subordinate") {
            senderPriority = 4;
        }
        else if (sender == "Peer") {
            senderPriority = 3;
        }
        else if (sender == "ImportantPerson") {
            senderPriority = 2;
        }
        else {
            senderPriority = 1;
        }

        /*
         * Convert:
         *
         * MM-DD-YYYY
         *
         * into:
         *
         * YYYYMMDD
         *
         * Example:
         *
         * 01-03-2025
         *
         * becomes:
         *
         * 20250103
         *
         * This makes dates easy to compare.
         */
        int month;
        int day;
        int year;

        char dash1;
        char dash2;

        stringstream dateStream(date);

        dateStream >> month
                   >> dash1
                   >> day
                   >> dash2
                   >> year;

        dateValue = year * 10000
                  + month * 100
                  + day;
    }


    /*
     * Determines whether this email has higher priority
     * than another email.
     */
    bool hasHigherPriorityThan(const Email& other) const {

        /*
         * First compare sender category.
         */
        if (senderPriority != other.senderPriority) {

            return senderPriority > other.senderPriority;
        }


        /*
         * If they have the same sender category,
         * the newer email gets priority.
         */
        if (dateValue != other.dateValue) {

            return dateValue > other.dateValue;
        }


        /*
         * If category AND date are identical,
         * keep the email that entered first ahead.
         *
         * This simply gives predictable behavior
         * when two emails are otherwise identical.
         */
        return arrivalOrder < other.arrivalOrder;
    }


    /*
     * Displays the email in the required format.
     */
    void display() const {

        cout << "Sender: " << sender << endl;
        cout << "Subject: " << subject << endl;
        cout << "Date: " << date << endl;
    }
};



/*
 * MaxHeap object
 *
 * Implements a MaxHeap using a list/array-based representation.
 *
 * No built-in C++ heap functions are used.
 */
class MaxHeap {
private:

    /*
     * Vector acts as the list used to store the heap.
     *
     * For index i:
     *
     * Parent      = (i - 1) / 2
     * Left child  = 2i + 1
     * Right child = 2i + 2
     */
    vector<Email> heap;


    /*
     * Swaps two Email objects inside the heap.
     */
    void swapEmails(int first, int second) {

        Email temp = heap[first];

        heap[first] = heap[second];

        heap[second] = temp;
    }


    /*
     * Moves a newly inserted email upward until
     * the MaxHeap property is restored.
     */
    void bubbleUp(int index) {

        while (index > 0) {

            int parent = (index - 1) / 2;


            /*
             * If the child has greater priority than
             * the parent, swap them.
             */
            if (heap[index].hasHigherPriorityThan(heap[parent])) {

                swapEmails(index, parent);

                index = parent;
            }
            else {

                break;
            }
        }
    }


    /*
     * Moves an email downward after the root is removed.
     */
    void bubbleDown(int index) {

        int heapSize = static_cast<int>(heap.size());


        while (true) {

            int leftChild = 2 * index + 1;

            int rightChild = 2 * index + 2;

            int highest = index;


            /*
             * Check the left child.
             */
            if (leftChild < heapSize &&
                heap[leftChild].hasHigherPriorityThan(heap[highest])) {

                highest = leftChild;
            }


            /*
             * Check the right child.
             */
            if (rightChild < heapSize &&
                heap[rightChild].hasHigherPriorityThan(heap[highest])) {

                highest = rightChild;
            }


            /*
             * If index is still the highest priority,
             * the heap is correct.
             */
            if (highest == index) {

                break;
            }


            swapEmails(index, highest);

            index = highest;
        }
    }


public:

    /*
     * Inserts an Email into the MaxHeap.
     */
    void insert(const Email& email) {

        /*
         * Put new email at end of list.
         */
        heap.push_back(email);


        /*
         * Restore heap property.
         */
        bubbleUp(static_cast<int>(heap.size()) - 1);
    }


    /*
     * Returns the highest-priority email
     * without removing it.
     */
    const Email* peek() const {

        if (heap.empty()) {

            return nullptr;
        }


        /*
         * Highest-priority item in a MaxHeap
         * is always index 0.
         */
        return &heap[0];
    }


    /*
     * Removes the highest-priority email.
     */
    void removeMax() {

        /*
         * Nothing to remove.
         */
        if (heap.empty()) {

            return;
        }


        /*
         * If only one email exists, simply remove it.
         */
        if (heap.size() == 1) {

            heap.pop_back();

            return;
        }


        /*
         * Move last email into the root.
         */
        heap[0] = heap.back();


        /*
         * Remove last position.
         */
        heap.pop_back();


        /*
         * Restore MaxHeap property.
         */
        bubbleDown(0);
    }


    /*
     * Returns number of emails currently in heap.
     */
    int count() const {

        return static_cast<int>(heap.size());
    }


    /*
     * Determines whether the heap is empty.
     */
    bool empty() const {

        return heap.empty();
    }
};



/*
 * CEOInbox object
 *
 * Handles commands from the input file and uses
 * a MaxHeap object to manage the CEO's emails.
 */
class CEOInbox {
private:

    MaxHeap inbox;

    long long nextArrivalOrder;


    /*
     * Removes spaces, tabs, and line endings
     * from the beginning and end of a string.
     */
    string trim(const string& text) const {

        string whitespace = " \t\r\n";


        size_t start =
            text.find_first_not_of(whitespace);


        /*
         * String contains only whitespace.
         */
        if (start == string::npos) {

            return "";
        }


        size_t end =
            text.find_last_not_of(whitespace);


        return text.substr(
            start,
            end - start + 1
        );
    }


    /*
     * Handles an EMAIL command.
     *
     * Example:
     *
     * EMAIL Boss,Never Mind,01-03-2025
     */
    void addEmail(const string& line) {

        /*
         * Remove "EMAIL " from the beginning.
         */
        string emailData = line.substr(6);


        /*
         * Find the two commas.
         */
        size_t firstComma =
            emailData.find(',');

        size_t secondComma =
            emailData.find(',', firstComma + 1);


        /*
         * Invalid input protection.
         */
        if (firstComma == string::npos ||
            secondComma == string::npos) {

            return;
        }


        /*
         * Extract sender.
         */
        string sender = trim(
            emailData.substr(
                0,
                firstComma
            )
        );


        /*
         * Extract subject.
         *
         * Subjects may contain spaces,
         * so we cannot simply use >>.
         */
        string subject = trim(
            emailData.substr(
                firstComma + 1,
                secondComma - firstComma - 1
            )
        );


        /*
         * Extract date.
         */
        string date = trim(
            emailData.substr(secondComma + 1)
        );


        /*
         * Create Email object.
         */
        Email newEmail(
            sender,
            subject,
            date,
            nextArrivalOrder
        );


        nextArrivalOrder++;


        /*
         * Insert Email object into heap.
         */
        inbox.insert(newEmail);
    }


    /*
     * Handles NEXT command.
     *
     * NEXT only displays the highest-priority email.
     *
     * It DOES NOT remove it.
     */
    void showNext() const {

        const Email* nextEmail =
            inbox.peek();


        /*
         * Handle NEXT with no emails.
         */
        if (nextEmail == nullptr) {

            cout << "No emails to read." << endl;

            return;
        }


        cout << "Next email:" << endl;


        nextEmail->display();
    }


    /*
     * Handles READ command.
     *
     * READ removes the highest-priority email.
     *
     * It does not matter whether NEXT was called first.
     */
    void readNext() {

        inbox.removeMax();
    }


    /*
     * Handles COUNT command.
     */
    void showCount() const {

        cout << "There are "
             << inbox.count()
             << " emails to read."
             << endl;
    }


public:

    /*
     * Constructor
     */
    CEOInbox() {

        nextArrivalOrder = 0;
    }


    /*
     * Reads and executes every command from
     * the input stream.
     */
    void processStream(istream& input) {

        string line;


        while (getline(input, line)) {

            /*
             * Remove extra whitespace or Windows \r.
             */
            line = trim(line);


            if (line.empty()) {

                continue;
            }


            /*
             * EMAIL command
             */
            if (line.rfind("EMAIL ", 0) == 0) {

                addEmail(line);
            }


            /*
             * NEXT command
             */
            else if (line == "NEXT") {

                showNext();
            }


            /*
             * READ command
             */
            else if (line == "READ") {

                readNext();
            }


            /*
             * COUNT command
             */
            else if (line == "COUNT") {

                showCount();
            }
        }
    }
};



/*
 * Main program
 */
int main(int argc, char* argv[]) {

    /*
     * Create CEO Inbox object.
     */
    CEOInbox ceoInbox;


    /*
     * If a filename is given:
     *
     * ./program test.txt
     *
     * read commands from that file.
     */
    if (argc >= 2) {

        ifstream inputFile(argv[1]);


        /*
         * Make sure file opened successfully.
         */
        if (!inputFile.is_open()) {

            cerr << "Unable to open input file."
                 << endl;

            return 1;
        }


        /*
         * Process commands.
         */
        ceoInbox.processStream(inputFile);
    }


    /*
     * This also allows:
     *
     * ./program < test.txt
     */
    else {

        ceoInbox.processStream(cin);
    }


    return 0;
}