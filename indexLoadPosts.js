function loadPosts(folder) {
    //Finds the element div#mainPage to put articles
    let mainPage = document.getElementById('mainPage');
    //Check 'list.txt' that contains lists of posts in 'folder'
    let listRequest = new XMLHttpRequest();
    listRequest.onreadystatechange = function() {
        if (listRequest.readyState == XMLHttpRequest.DONE) {
            mainPage.innerHTML = '';
            if (listRequest.status === 0 || (listRequest.status >= 200 && listRequest.status < 400)) {
                let list = listRequest.responseText;
                console.log('Got list of posts in ' + folder + ': \n' + listRequest.responseText);
                if (list !== '') {
                    let posts = list.split('\n');
                    for (let i = 0; i < posts.length; i++) {
                        let postRequest = new XMLHttpRequest();
                        postRequest.onreadystatechange = function() {
                            if (postRequest.readyState == XMLHttpRequest.DONE) {
                                if (postRequest === 0 || (postRequest.status >= 200 && postRequest.status < 400)) {
                                    mainPage.innerHTML += '<section>' + postRequest.responseText + '</section>';
                                }
                            }
                        }
                        postRequest.open('GET', folder + '/' + posts[i]);
                        postRequest.send();
                    }
                }
                else {
                    mainPage.innerHTML += '<section style="text-align:center">작성한 포스트가 없습니다.</section>'
                }
            }
            else {
                mainPage.innerHTML += '<section style="text-align:center">포스트를 불러오는 중 오류가 발생하였습니다.</section>'
            }
        }
    };
    listRequest.open('GET', folder + '/list.txt');
    listRequest.send();
}