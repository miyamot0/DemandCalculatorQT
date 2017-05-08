if(suppressWarnings(suppressMessages((!require("reshape", quietly = TRUE))))) {
    suppressWarnings(suppressMessages((install.packages("reshape", repos='http://cran.us.r-project.org', dependencies = TRUE))))
    suppressWarnings(suppressMessages((require("reshape", quietly = TRUE))))
}

response <- suppressWarnings(suppressMessages((require("reshape", quietly = TRUE))))

response
