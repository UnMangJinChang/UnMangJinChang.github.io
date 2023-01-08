/**@type string[] */
let posts;
/**@type number */
let pagesNum = 0;
/**@type number */
let currentPageIdx = 0;
/**@type string */
let currentFolder = '';
/**@type number */
const SECTION_PER_PAGE = 5;

function setPageSelector(/**@type HTMLElement*/pageSelect) {
    pageSelect.innerHTML = '';
    if (pagesNum === 0) {
        return;
    }
    if (pagesNum <= 5) {
        pageSelect.innerHTML = '';
        for (let i = 1; i <= pagesNum; i++) {
            pageSelect.innerHTML += "<a href='javascript:;' onclick='showPosts(parseInt(this.innerText) - 1)'>" + (i - 1 == currentPageIdx ? '<em>' + i + '</em>' : i) + "</a>\t";
            if (i !== pagesNum) {
                pageSelect.innerHTML += "\t";
            }
        }
    }
    else {
        pageSelect.innerHTML = '';
        for (let i = Math.max(1, currentPageIdx - 1); i <= Math.min(pagesNum, currentPageIdx + 3); i++) {
            pageSelect.innerHTML += "<a href='javascript:;' onclick='showPosts(parseInt(this.innerText) - 1)'>" + (i - 1 == currentPageIdx ? '<em>' + i + '</em>' : i) + "</a>\t";
        }
        pageSelect.innerHTML += "\t\t";
        pageSelect.innerHTML += "<input type='number' value='' min='1' max='" + pagesNum + "' onkeydown='function(event) {if (event.keyCode == 13) {showPosts(parseInt(this.value) - 1);}}'>";
    }
}

function showPosts(pageIdx) {
    pagesNum = posts.length % SECTION_PER_PAGE === 0 ? Math.floor(posts.length / SECTION_PER_PAGE) : Math.floor(posts.length / SECTION_PER_PAGE) + 1;
    let mainPage = document.getElementById('mainPage');
    let main = mainPage.getElementsByClassName('page').item(0);
    let pageSelect = mainPage.getElementsByClassName('pageSelect').item(0);
    if (pagesNum === 0) {
        main.innerHTML = '<section style="text-align:center">작성한 포스트가 없습니다.</section>'
    }
    if (pageIdx >= pagesNum || pageIdx < 0) {
        return;
    }
    currentPageIdx = pageIdx;
    main.innerHTML = '';
    for (let i = SECTION_PER_PAGE * pageIdx; i < Math.min(SECTION_PER_PAGE * (pageIdx + 1), posts.length); i++) {
        let postRequest = new XMLHttpRequest();
        postRequest.onreadystatechange = function() {
            if (postRequest.readyState == XMLHttpRequest.DONE) {
                if (postRequest.status === 0 || (postRequest.status >= 200 && postRequest.status < 400)) {
                    main.innerHTML += '<section>' + postRequest.responseText + '</section>';
                }
            }
        }
        postRequest.open('GET', currentFolder + '/' + posts[i], false);
        postRequest.send();
    }
    setPageSelector(pageSelect);
    MathJax.typeset();
    hljs.highlightAll();
}

function loadPosts(folder) {
    currentFolder = folder;
    //Finds the element div#mainPage to put articles
    let mainPage = document.getElementById('mainPage');
    let main = mainPage.getElementsByClassName('page').item(0);
    let pageSelect = mainPage.getElementsByClassName('pageSelect').item(0);
    //Check 'list.txt' that contains lists of posts in 'folder'
    let listRequest = new XMLHttpRequest();
    listRequest.onreadystatechange = function() {
        if (listRequest.readyState == XMLHttpRequest.DONE) {
            main.innerHTML = '';
            if (listRequest.status === 0 || (listRequest.status >= 200 && listRequest.status < 400)) {
                let list = listRequest.responseText;
                if (list !== '') {
                    posts = list.split('\n');
                    posts.pop();
                    showPosts(0);
                }
                else {
                    main.innerHTML += '<section style="text-align:center">작성한 포스트가 없습니다.</section>'
                    posts = [];
                    setPageSelector(pageSelect);
                }
            }
            else {
                main.innerHTML += '<section style="text-align:center">포스트를 불러오는 중 오류가 발생하였습니다.</section>'
                posts = [];
                setPageSelector(pageSelect);
            }
        }
    };
    listRequest.open('GET', folder + '/list.txt', false);
    listRequest.send();
}

function searchPosts(folder) {
    /**@type string*/
    let query = document.getElementsByClassName('searchInput').item(0).value;currentFolder = folder;
    //Finds the element div#mainPage to put articles
    let mainPage = document.getElementById('mainPage');
    let main = mainPage.getElementsByClassName('page').item(0);
    let pageSelect = mainPage.getElementsByClassName('pageSelect').item(0);
    //Check 'list.txt' that contains lists of posts in 'folder'
    let listRequest = new XMLHttpRequest();
    listRequest.onreadystatechange = function() {
        if (listRequest.readyState == XMLHttpRequest.DONE) {
            main.innerHTML = '';
            if (listRequest.status === 0 || (listRequest.status >= 200 && listRequest.status < 400)) {
                let list = listRequest.responseText;
                if (list !== '') {
                    posts = list.split('\n');
                    posts.pop();
                }
            }
        }
    };
    listRequest.open('GET', folder + '/list.txt', false);
    listRequest.send();
    query = query.replace(/\s/g, '').toLocaleLowerCase();
    //Filter out posts not containing the query string.
    for (let i = 0; i < posts.length; i++) {
        let postRequest = new XMLHttpRequest();
        postRequest.onreadystatechange = function() {
            if (postRequest.readyState == XMLHttpRequest.DONE) {
                if (postRequest.status === 0 || (postRequest.status >= 200 && postRequest.status < 400)) {
                    const target = (new DOMParser()).parseFromString(postRequest.responseText, 'text/html').body.innerText.replace(/\s/g, '').toLocaleLowerCase();
                    if (!target.includes(query)) {
                        posts.splice(i, 1);
                        i--;
                    }
                }
            }
        }
        postRequest.open('GET', currentFolder + '/' + posts[i], false);
        postRequest.send();
    }
    showPosts(0);
}