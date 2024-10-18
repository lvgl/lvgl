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

    fetch('https://lvgl.io/home-banner.txt') // Replace with your URL
  .then(response => {
    // Check if the request was successful
    if (!response.ok) {
      throw new Error(`HTTP error! Status: ${response.status}`);
    }
    // Read the response as text
    return response.text();
  })
  .then(data => {

    const section = document.querySelector('.wy-nav-content-wrap');

    //Add a div
    const newDiv = document.createElement('div');
    newDiv.style="background-image: linear-gradient(45deg, black, #5e5e5e); color: white; border-bottom: 4px solid #e10010; padding-inline:3em"
    section.insertBefore(newDiv, section.firstChild);


    //Add a p to the div
    const newP = document.createElement('p');
    newP.style="padding-block:12px; margin-block:0px;align-content: center;align-items: center;"
    newP.innerHTML = data
    newDiv.insertBefore(newP, newDiv.firstChild);

    const children = newDiv.querySelectorAll('*');

    // Iterate over each child
    children.forEach(child => {
        // Check if the child has an id
        if (child.id) {
            // Prepend 'docs-' to the id
            child.id = 'docs-' + child.id;
        }
    })
  }) .catch(error => {
        console.error('Fetch error: ' + error.message);
  });
})
