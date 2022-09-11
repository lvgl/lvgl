/* API collapsing */
document.addEventListener('DOMContentLoaded', (event) => {
    document.querySelectorAll("dl.cpp").forEach(cppListing => {
        const dt = cppListing.querySelector("dt");
        let shouldBeExpanded = false;
        if(dt.id == document.location.hash.substring(1))
            shouldBeExpanded = true;
        cppListing.classList.add(shouldBeExpanded ? "expanded" : "unexpanded");
        const button = document.createElement("span");
        button.classList.add("lv-api-expansion-button");
        button.addEventListener("click", () => {
            cppListing.classList.toggle("unexpanded");
            cppListing.classList.toggle("expanded");
        });
        
        dt.insertBefore(button, dt.firstChild);
    });
})