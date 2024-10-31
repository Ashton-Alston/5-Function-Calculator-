#include <windows.h>
#include <tchar.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <CommCtrl.h> 
#pragma comment(lib, "comctl32.lib") // Automatically links with the comctl32.lib library, necessary for using common controls like the tab control.

// Define control IDs for buttons, text box, and other UI elements.
#define ID_BUTTON_7 2
#define ID_BUTTON_8 3
#define ID_BUTTON_9 4
#define ID_BUTTON_4 5
#define ID_BUTTON_5 6
#define ID_BUTTON_6 7
#define ID_BUTTON_1 8
#define ID_BUTTON_2 9
#define ID_BUTTON_3 10
#define ID_BUTTON_0 11
#define ID_BUTTON_PLUS 12
#define ID_BUTTON_MINUS 13
#define ID_BUTTON_MULTIPLY 14
#define ID_BUTTON_DIVIDE 15
#define ID_BUTTON_PERCENT 16
#define ID_BUTTON_EQUALS 17
#define ID_BUTTON_DOT 18
#define ID_BUTTON_CLEAR 19
#define ID_BUTTON_CLEARENTRY 20
#define ID_TEXTBOX 21
#define ID_BUTTON_INTRO_TRACE_ON 22
#define ID_BUTTON_INTRO_TRACE_OFF 23
#define ID_TraceEdit 24

// Global variables for window management and UI components.
static TCHAR szWindowClass[] = _T("CalculatorApp"); // Unique name for the window class.
static TCHAR szTitle[] = _T("Calculator"); // Title of the window displayed in the title bar.
HINSTANCE hInst; // Stores the current instance of the application.
// Handles to various UI components.
HWND hTab;
HWND hTraceEdit;
HWND hButton7, hButton8, hButton9, hButton4, hButton5, hButton6;
HWND hButton1, hButton2, hButton3, hButton0;
HWND hButtonPlus, hButtonMinus, hButtonMultiply, hButtonDivide, hButtonPercent, hButtonEquals, hButtonDot;
HWND hButtonClear, hButtonClearEntry, hTextBox;
int someVariable = 0; // An example variable for demonstration purposes (should be used or removed in a real application).
int decisionCount = 0; // Counter for decision points, useful for tracking user interactions or logical decisions.
bool tracingEnabled = true; // Flag to enable/disable tracing functionality.

HWND hIntroOn;  // Handle for the button to enable introduction tracing.
HWND hIntroOff; // Handle for the button to disable introduction tracing.

// Function to log messages to the Trace Edit control. 
// It appends messages to the control, useful for debugging or user interaction tracing.
void LogToTrace(HWND hTraceEdit, const std::wstring& message) {
    if (tracingEnabled) {
        int length = GetWindowTextLength(hTraceEdit); // Get the current length of the text.
        SendMessage(hTraceEdit, EM_SETSEL, length, length); // Set the selection to the end of the text.
        SendMessage(hTraceEdit, EM_REPLACESEL, 0, (LPARAM)message.c_str()); // Append the message.
        SendMessage(hTraceEdit, EM_REPLACESEL, 0, (LPARAM)L"\r\n"); // Append a new line.
    }
}

// Forward declaration of functions used by the application.
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
double EvaluateExpression(const std::wstring& expression);

// Entry point of the Windows application. 
int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
) {
    // Initialize common controls to use modern UI elements like tabs.
    INITCOMMONCONTROLSEX icex = { 0 };
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES;
    InitCommonControlsEx(&icex);

    // Define and register the window class.
    WNDCLASSEX wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, _T("Window class creation failed!"), _T("Error"), MB_ICONERROR);
        return 1;
    }

    // Create the main application window.
    hInst = hInstance; // Store instance handle in our global variable.
    HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 350, 400, NULL, NULL, hInstance, NULL);
    if (!hWnd) {
        MessageBox(NULL, _T("Call to CreateWindow failed!"), szTitle, NULL);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Main message loop of the application.
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// Function to evaluate a simple arithmetic expression.
// Parses the expression from a string and computes the result based on the operation.
double EvaluateExpression(const std::wstring& expression) {
    std::wistringstream iss(expression);
    double num1, num2;
    wchar_t op;
    bool isPercentage = false;

    iss >> num1 >> op >> num2; // Attempt to parse the first number, operator, and second number from the expression.

    if (expression.back() == L'%') { // Check for a percentage operation.
        isPercentage = true;
        num2 /= 100; // Convert percentage to a decimal.
    }

    // Perform the calculation based on the operator.
    switch (op) {
    case L'+': return isPercentage ? num1 + (num1 * num2) : num1 + num2;
    case L'-': return isPercentage ? num1 - (num1 * num2) : num1 - num2;
    case L'*': return num1 * num2;
    case L'/': return num2 == 0 ? NAN : num1 / num2; // Handle division by zero.
    default: return NAN; // Handle unknown operator.
    }
}


// Window procedure function to handle window messages.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        // Tab control creation
        hTab = CreateWindow(WC_TABCONTROL, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0, 500, 400, hWnd, NULL, hInst, NULL);

        TCITEM tie = { 0 };
        tie.mask = TCIF_TEXT;

        wchar_t tab1Title[] = L"Calculator";
        wchar_t tab2Title[] = L"Trace Logic";

        tie.pszText = tab1Title;
        TabCtrl_InsertItem(hTab, 0, &tie);

        tie.pszText = tab2Title;
        TabCtrl_InsertItem(hTab, 1, &tie);

        hTraceEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_READONLY, 10, 30, 480, 330, hWnd, NULL, hInst, NULL);
        ShowWindow(hTraceEdit, SW_HIDE);

        // Create calculator buttons
        hButton7 = CreateWindow(L"BUTTON", L"7", WS_CHILD | WS_VISIBLE | BS_FLAT, 10, 110, 50, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_7, hInst, NULL);
        hButton8 = CreateWindow(L"BUTTON", L"8", WS_CHILD | WS_VISIBLE | BS_FLAT, 70, 110, 50, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_8, hInst, NULL);
        hButton9 = CreateWindow(L"BUTTON", L"9", WS_CHILD | WS_VISIBLE | BS_FLAT, 130, 110, 50, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_9, hInst, NULL);
        hButton4 = CreateWindow(L"BUTTON", L"4", WS_CHILD | WS_VISIBLE | BS_FLAT, 10, 170, 50, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_4, hInst, NULL);
        hButton5 = CreateWindow(L"BUTTON", L"5", WS_CHILD | WS_VISIBLE | BS_FLAT, 70, 170, 50, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_5, hInst, NULL);
        hButton6 = CreateWindow(L"BUTTON", L"6", WS_CHILD | WS_VISIBLE | BS_FLAT, 130, 170, 50, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_6, hInst, NULL);
        hButton1 = CreateWindow(L"BUTTON", L"1", WS_CHILD | WS_VISIBLE | BS_FLAT, 10, 230, 50, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_1, hInst, NULL);
        hButton2 = CreateWindow(L"BUTTON", L"2", WS_CHILD | WS_VISIBLE | BS_FLAT, 70, 230, 50, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_2, hInst, NULL);
        hButton3 = CreateWindow(L"BUTTON", L"3", WS_CHILD | WS_VISIBLE | BS_FLAT, 130, 230, 50, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_3, hInst, NULL);
        hButton0 = CreateWindow(L"BUTTON", L"0", WS_CHILD | WS_VISIBLE | BS_FLAT, 10, 290, 110, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_0, hInst, NULL);
        hButtonPlus = CreateWindow(L"BUTTON", L"+", WS_CHILD | WS_VISIBLE | BS_FLAT, 210, 170, 50, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_PLUS, hInst, NULL);
        hButtonMinus = CreateWindow(L"BUTTON", L"-", WS_CHILD | WS_VISIBLE | BS_FLAT, 210, 230, 50, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_MINUS, hInst, NULL);
        hButtonMultiply = CreateWindow(L"BUTTON", L"*", WS_CHILD | WS_VISIBLE | BS_FLAT, 210, 290, 50, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_MULTIPLY, hInst, NULL);
        hButtonDivide = CreateWindow(L"BUTTON", L"/", WS_CHILD | WS_VISIBLE | BS_FLAT, 270, 170, 50, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_DIVIDE, hInst, NULL);
        hButtonPercent = CreateWindow(L"BUTTON", L"%", WS_CHILD | WS_VISIBLE | BS_FLAT, 270, 230, 50, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_PERCENT, hInst, NULL);
        hButtonEquals = CreateWindow(L"BUTTON", L"=", WS_CHILD | WS_VISIBLE | BS_FLAT, 270, 290, 50, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_EQUALS, hInst, NULL);
        hButtonDot = CreateWindow(L"BUTTON", L".", WS_CHILD | WS_VISIBLE | BS_FLAT, 130, 290, 50, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_DOT, hInst, NULL);
        hButtonClear = CreateWindow(L"BUTTON", L"C", WS_CHILD | WS_VISIBLE | BS_FLAT, 210, 110, 50, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_CLEAR, hInst, NULL);
        hButtonClearEntry = CreateWindow(L"BUTTON", L"CE", WS_CHILD | WS_VISIBLE | BS_FLAT, 270, 110, 50, 50, hWnd, (HMENU)(INT_PTR)ID_BUTTON_CLEARENTRY, hInst, NULL);
        hTextBox = CreateWindowEx(
            WS_EX_CLIENTEDGE,       // Extended window style
            L"EDIT",                // Predefined class; edit control
            L"",                    // No default text
            WS_CHILD | WS_VISIBLE | ES_RIGHT | ES_AUTOHSCROLL, // Styles: child window, initially visible, left-aligned text, automatically scrolls
            10, 70, 310, 20,        // Set position and size (x, y, width, height)
            hWnd,                   // Parent window
            (HMENU)(INT_PTR)ID_TEXTBOX, // Control ID
            hInst,                  // Instance
            NULL                    // No additional application data
        );

        // "Intro Trace On" button creation
        hIntroOn = CreateWindow(L"BUTTON", L"Intro Trace On", WS_CHILD | WS_VISIBLE | BS_FLAT,
            110, 30, 110, 20, hWnd, (HMENU)(INT_PTR)ID_BUTTON_INTRO_TRACE_ON, hInst, NULL);

        // "Intro Trace Off" button creation
        hIntroOff = CreateWindow(L"BUTTON", L"Intro Trace Off", WS_CHILD | WS_VISIBLE | BS_FLAT,
            220, 30, 110, 20, hWnd, (HMENU)(INT_PTR)ID_BUTTON_INTRO_TRACE_OFF, hInst, NULL);
        hTraceEdit = CreateWindowEx(
            WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | WS_VSCROLL,
            10, 30, 480, 330, hWnd, (HMENU)(INT_PTR)ID_TraceEdit, hInst, NULL);

        // In WM_CREATE, after creating hTraceEdit
        ShowWindow(hTraceEdit, SW_HIDE); // Initially hide the Trace Edit control


    }
                  // Handle notifications, for example, tab control selection changes.
    case WM_NOTIFY: {
        NMHDR* pNmHdr = (NMHDR*)lParam;
        if (pNmHdr->hwndFrom == hTab && pNmHdr->code == TCN_SELCHANGE) {
            int selectedTab = TabCtrl_GetCurSel(hTab);

            // Assuming the first tab is the calculator and the second is "Trace Logic"
            BOOL showButtons = (selectedTab == 0); // Show calculator buttons on the first tab
            BOOL showTrace = (selectedTab == 1); // Show Trace Edit on the second tab

            ShowWindow(hButton0, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hButton1, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hButton2, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hButton3, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hButton4, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hButton5, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hButton6, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hButton7, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hButton8, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hButton9, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hButtonPlus, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hButtonMinus, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hButtonMultiply, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hButtonDivide, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hButtonPercent, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hButtonEquals, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hButtonDot, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hButtonClear, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hButtonClearEntry, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hTextBox, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hIntroOn, showButtons ? SW_SHOW : SW_HIDE);
            ShowWindow(hIntroOff, showButtons ? SW_SHOW : SW_HIDE);

            // Show or hide the Trace Edit control
            ShowWindow(hTraceEdit, showTrace ? SW_SHOW : SW_HIDE);
        }
        break;
    }
                  // Handle commands, typically button clicks.
    case WM_COMMAND: {
        WORD wmId = LOWORD(wParam); // Get the identifier of the clicked button

        switch (wmId) {
            // Digit buttons
        case ID_BUTTON_0:
            SendMessage(hTextBox, EM_SETSEL, WPARAM(-1), LPARAM(-1)); // Move caret to the end
            SendMessage(hTextBox, EM_REPLACESEL, FALSE, (LPARAM)L"0"); // Append '0'
            LogToTrace(hTraceEdit, L"Button '0' pressed.");
            break;
        case ID_BUTTON_1:
            SendMessage(hTextBox, EM_SETSEL, WPARAM(-1), LPARAM(-1));
            SendMessage(hTextBox, EM_REPLACESEL, FALSE, (LPARAM)L"1"); // Append '1'
            LogToTrace(hTraceEdit, L"Button '1' pressed.");
            break;
        case ID_BUTTON_2:
            SendMessage(hTextBox, EM_SETSEL, WPARAM(-1), LPARAM(-1));
            SendMessage(hTextBox, EM_REPLACESEL, FALSE, (LPARAM)L"2"); // Append '2'
            LogToTrace(hTraceEdit, L"Button '2' pressed.");
            break;
        case ID_BUTTON_3:
            SendMessage(hTextBox, EM_SETSEL, WPARAM(-1), LPARAM(-1));
            SendMessage(hTextBox, EM_REPLACESEL, FALSE, (LPARAM)L"3"); // Append '3'
            LogToTrace(hTraceEdit, L"Button '3' pressed.");
            break;
        case ID_BUTTON_4:
            SendMessage(hTextBox, EM_SETSEL, WPARAM(-1), LPARAM(-1));
            SendMessage(hTextBox, EM_REPLACESEL, FALSE, (LPARAM)L"4"); // Append '4'
            LogToTrace(hTraceEdit, L"Button '4' pressed.");
            break;
        case ID_BUTTON_5:
            SendMessage(hTextBox, EM_SETSEL, WPARAM(-1), LPARAM(-1));
            SendMessage(hTextBox, EM_REPLACESEL, FALSE, (LPARAM)L"5"); // Append '5'
            LogToTrace(hTraceEdit, L"Button '5' pressed.");
            break;
        case ID_BUTTON_6:
            SendMessage(hTextBox, EM_SETSEL, WPARAM(-1), LPARAM(-1));
            SendMessage(hTextBox, EM_REPLACESEL, FALSE, (LPARAM)L"6"); // Append '6'
            LogToTrace(hTraceEdit, L"Button '6' pressed.");
            break;
        case ID_BUTTON_7:
            SendMessage(hTextBox, EM_SETSEL, WPARAM(-1), LPARAM(-1));
            SendMessage(hTextBox, EM_REPLACESEL, FALSE, (LPARAM)L"7"); // Append '7'
            LogToTrace(hTraceEdit, L"Button '7' pressed.");
            break;
        case ID_BUTTON_8:
            SendMessage(hTextBox, EM_SETSEL, WPARAM(-1), LPARAM(-1));
            SendMessage(hTextBox, EM_REPLACESEL, FALSE, (LPARAM)L"8"); // Append '8'
            LogToTrace(hTraceEdit, L"Button '8' pressed.");
            break;
        case ID_BUTTON_9:
            SendMessage(hTextBox, EM_SETSEL, WPARAM(-1), LPARAM(-1));
            SendMessage(hTextBox, EM_REPLACESEL, FALSE, (LPARAM)L"9"); // Append '9'
            LogToTrace(hTraceEdit, L"Button '9' pressed.");
            break;
            // Operation buttons
        case ID_BUTTON_PLUS:
            SendMessage(hTextBox, EM_SETSEL, WPARAM(-1), LPARAM(-1));
            SendMessage(hTextBox, EM_REPLACESEL, FALSE, (LPARAM)L"+");
            LogToTrace(hTraceEdit, L"Button '+' pressed.");
            break;
        case ID_BUTTON_MINUS:
            SendMessage(hTextBox, EM_SETSEL, WPARAM(-1), LPARAM(-1));
            SendMessage(hTextBox, EM_REPLACESEL, FALSE, (LPARAM)L"-");
            LogToTrace(hTraceEdit, L"Button '-' pressed.");
            break;
        case ID_BUTTON_MULTIPLY:
            SendMessage(hTextBox, EM_SETSEL, WPARAM(-1), LPARAM(-1));
            SendMessage(hTextBox, EM_REPLACESEL, FALSE, (LPARAM)L"*");
            LogToTrace(hTraceEdit, L"Button '*' pressed.");
            break;
        case ID_BUTTON_DIVIDE:
            SendMessage(hTextBox, EM_SETSEL, WPARAM(-1), LPARAM(-1));
            SendMessage(hTextBox, EM_REPLACESEL, FALSE, (LPARAM)L"/");
            LogToTrace(hTraceEdit, L"Button '/' pressed.");
            break;
        case ID_BUTTON_PERCENT:
            SendMessage(hTextBox, EM_SETSEL, WPARAM(-1), LPARAM(-1));
            SendMessage(hTextBox, EM_REPLACESEL, FALSE, (LPARAM)L"%");
            LogToTrace(hTraceEdit, L"Button '%' pressed.");
            break;
        case ID_BUTTON_DOT:
            SendMessage(hTextBox, EM_SETSEL, WPARAM(-1), LPARAM(-1));
            SendMessage(hTextBox, EM_REPLACESEL, FALSE, (LPARAM)L".");
            LogToTrace(hTraceEdit, L"Button '.' pressed.");
            break;
        case ID_BUTTON_CLEAR:
            SetWindowText(hTextBox, L"");
            LogToTrace(hTraceEdit, L"Button 'C' pressed. Calculator cleared.");
            break;
        case ID_BUTTON_INTRO_TRACE_ON: {
            tracingEnabled = true; // Enable tracing
            MessageBox(hWnd, L"Intro Trace is now ON.", L"Trace Status", MB_OK | MB_ICONINFORMATION);
            break;
        }

        case ID_BUTTON_INTRO_TRACE_OFF: {
            tracingEnabled = false; // Disable tracing
            MessageBox(hWnd, L"Intro Trace is now OFF.", L"Trace Status", MB_OK | MB_ICONINFORMATION);
            break;
        }


        case ID_BUTTON_CLEARENTRY: {
            WCHAR currentText[256];
            GetWindowText(hTextBox, currentText, 256);
            std::wstring text(currentText);
            LogToTrace(hTraceEdit, L"Button 'CE' pressed. Entry cleared.");

            if (!text.empty()) {
                // Find the last non-digit character, which signifies the end of an operation or start of the last number
                size_t pos = text.find_last_not_of(L"0123456789");
                if (pos == std::wstring::npos) {
                    // If no non-digit character found, clear the whole text
                    text.clear();
                }
                else if (pos == text.length() - 1) {
                    // If the last character is a non-digit, remove it (it's an operation)
                    text.erase(pos);
                }
                else {
                    // If a non-digit character is found, erase the last number (everything after the last non-digit character)
                    text.erase(pos + 1);
                }

                SetWindowText(hTextBox, text.c_str());
            }
            break;
        }

        case ID_BUTTON_EQUALS: {
            WCHAR expression[256];
            GetWindowText(hTextBox, expression, 256);
            std::wstring exprStr(expression);
            LogToTrace(hTraceEdit, L"Evaluating expression: " + exprStr);
            double result = EvaluateExpression(exprStr);
            std::wstringstream ss;
            ss << result;
            SetWindowText(hTextBox, ss.str().c_str()); // Display result
            LogToTrace(hTraceEdit, L"Result: " + ss.str());
            break;
        }

        }
        break;
    }


                   // Terminate the application.
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }

    default: {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    }
    return 0;
}

