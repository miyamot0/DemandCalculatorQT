if(suppressWarnings(suppressMessages((!require("base64enc", quietly = TRUE))))) {
    suppressWarnings(suppressMessages((install.packages("base64enc", repos='http://cran.us.r-project.org', dependencies = TRUE))))
    suppressWarnings(suppressMessages((require("base64enc", quietly = TRUE))))
}

response <- suppressWarnings(suppressMessages((require("base64enc", quietly = TRUE))))

response
