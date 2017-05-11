if(suppressWarnings(suppressMessages((!require("devtools", quietly = TRUE))))) {
    suppressWarnings(suppressMessages((install.packages("devtools", repos='http://cran.us.r-project.org', dependencies = TRUE))))
    suppressWarnings(suppressMessages((require("devtools", quietly = TRUE))))
}

response <- suppressWarnings(suppressMessages((require("devtools", quietly = TRUE))))

response
