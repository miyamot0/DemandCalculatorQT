if(suppressWarnings(suppressMessages((!require("jsonlite", quietly = TRUE))))) {
    suppressWarnings(suppressMessages((install.packages("jsonlite", repos='http://cran.us.r-project.org', dependencies = TRUE))))
    suppressWarnings(suppressMessages((require("jsonlite", quietly = TRUE))))
}

response <- suppressWarnings(suppressMessages((require("jsonlite", quietly = TRUE))))

response
