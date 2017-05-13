suppressWarnings(suppressMessages(require("devtools")))
suppressWarnings(suppressMessages(require("digest")))

if(suppressWarnings(suppressMessages((!require("beezdemand", quietly = TRUE))))) {
    suppressWarnings(suppressMessages(devtools::install_github("brentkaplan/beezdemand", build_vignettes = TRUE)))
    suppressWarnings(suppressMessages((require("beezdemand", quietly = TRUE))))
}

response <- suppressWarnings(suppressMessages((require("beezdemand", quietly = TRUE))))

response
