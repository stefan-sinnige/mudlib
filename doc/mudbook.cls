% ===========================================================================
% LaTeX class style for Kalion PGL books.
% ===========================================================================
\NeedsTeXFormat{LaTeX2e}[1995/12/01]
\ProvidesClass{mudbook}[2016/08/24 v1.0 Kalion MUD Book document class]

% ---------------------------------------------------------------------------
% Document class options
% ---------------------------------------------------------------------------

% Pass every unknown option to the base class
\DeclareOption*{%
  \PassOptionsToClass{\CurrentOption}{book}%
}

% Process the options
\ProcessOptions\relax
\LoadClass{book}

% ---------------------------------------------------------------------------
% Additional packages
% ---------------------------------------------------------------------------

% Use the Computer Modern Bright fonts
\usepackage{cmbright}
\def\fontfamilyname{Computer Modern Bright}
\def\fontfamilylicense{\LaTeX Project Public License}

% Use the geometry package to control page layout easily
\usepackage[headheight=15pt]{geometry}

% Use the fancyhdr package to easily control headers and footers
\usepackage{fancyhdr}

% Use biblatex and biber for bibliography references. Use the main TeX file
% (as provided in \jobname) appended with .bib as the bibliography file.
\def\bibfile{bibliography.bib}
\usepackage[backend=biber,style=alphabetic]{biblatex}
\addbibresource{\bibfile}

% Index package
\usepackage{makeidx}

% Use the hyperref package to create PDF hyperlinks, but do not show them
% as links.
\usepackage[hidelinks]{hyperref}

% Use the relsize package to set font relative to the current font size.
\usepackage{relsize}

% Use the xspace package to add/eat spaces automatically
\usepackage{xspace}
 
% Use the xcolor package to add colors
\usepackage{xcolor}
 
% Use the listings package to format listing with highlighting
\usepackage{listings}

% Use the mud-uml package (which in turn uses tikz), for UML diagramming.
\usepackage{mud-uml}

% ---------------------------------------------------------------------------
% Commands (new and redefined)
% ---------------------------------------------------------------------------

% The MUD and C++ typeset. These are used to consistently format the language
% names correctly. The mbox is used to keep the names together.
\newcommand{\MUD}{MUD\xspace}
\newcommand{\RaisedPlus}{\protect\hspace{-.1em}\protect\raisebox{.35ex}{\smaller{\smaller\textbf{+}}}}
\newcommand{\Cpp}{\mbox{C\RaisedPlus\RaisedPlus}\xspace}

