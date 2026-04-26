/*
 * Simple C++ syntax highlighting
 */

/*
 * Highlight C++ keywords - replace occurences with a 'span' tag. Note that the
 * 'class' keyword is highlighted first, otehrwise it would highlight the span's
 * c;ass tag too.
 */
function highlight_cpp_keywords(text) {
    const keywords = [
        "class", "alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel",
        "atomic_commit", "atomic_noexcept", "auto", "bitand", "bitor", "bool",
        "break", "case", "catch", "char", "char8_t", "char16_t", "char32_t",
        "compl", "concept", "const", "consteval", "constexpr",
        "constinit", "const_cast", "continue", "contract_assert", "co_await",
        "co_return", "co_yield", "decltype", "default", "delete", "do",
        "double", "dynamic_cast", "else", "enum", "explicit", "export",
        "extern", "false", "float", "for", "friend", "goto", "if", "inline",
        "int", "long", "mutable", "namespace", "new", "noexcept", "not",
        "not_eq", "nullptr", "operator", "or", "or_eq", "private", "protected",
        "public", "reflexpr", "register", "reinterpret_cast", "requires",
        "return", "short", "signed", "sizeof", "static", "static_assert",
        "static_cast", "struct", "switch", "synchronized", "template", "this",
        "thread_local", "throw", "true", "try", "typedef", "typeid", "typename",
        "union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t",
        "while", "xor", "xor_eq",
        "final", "override", "transaction_safe", "transaction_safe_dynamic",
        "import", "module", "pre", "post"
    ];
    keywords.forEach((keyword, index) => {
        regex = new RegExp("\\b(" + keyword + ")\\b", "g");
        text = text.replace(
            regex,
            "<span class=\"highlight-keyword\">$1</span>");
        });
    return text;
}

/*
 * Highlight C++ comments - replace occurences with a 'span' tag
 */
function highlight_cpp_comments(text) {
    regex = new RegExp("(\/\/.*)", "g");
    text = text.replace(
        regex,
        "<span class=\"highlight-comment\">$1</span>");
    regex = new RegExp("(\/\\*[\\s\\S]*?\\*\/)", "g");
    text = text.replace(
        regex,
        "<span class=\"highlight-comment\">$1</span>");
    return text;
}

/*
 * Highlight the 'div' for C++:
 *    - Keywords
 *    - Comments
 */
function highlight_cpp(div) {
    text = div.innerHTML;
    text = highlight_cpp_keywords(text);
    text = highlight_cpp_comments(text);
    div.innerHTML = text;
}

/*
 * Highlight all '<div class="name">' sections.
 */
function highlight(name) {
    divs = document.querySelectorAll("div." + name);
    divs.forEach(
        div => {
            highlight_cpp(div);
        });
}

/*
 * Highlight when the document is loaded
 */
document.addEventListener(
    "DOMContentLoaded",
    (event) => {
        highlight("codeblock");
        highlight("example");
    })
