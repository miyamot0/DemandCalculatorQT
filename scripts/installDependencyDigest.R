if(suppressWarnings(suppressMessages((!require("digest", quietly = TRUE))))) {
    suppressWarnings(suppressMessages((install.packages("digest", repos='http://cran.us.r-project.org', dependencies = TRUE))))
    suppressWarnings(suppressMessages((require("digest", quietly = TRUE))))
}

response <- suppressWarnings(suppressMessages((require("digest", quietly = TRUE))))

response
