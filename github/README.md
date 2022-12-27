# Work github

## Авторизация по SSH ключу

Generate a SSH key pair (private/public):  
```
ssh-keygen -t rsa -C "your_email@example.com"
```
Settings => SSH and GPG keys => add ssh key  

Test the SSH key:  
```
ssh -T git@github.com
```
Change directory into the local clone of your repository (if you're not already there) and run:  
```
git remote set-url origin git@github.com:username/your-repository.git
```
Now try editing a file (try the README) and then do:  
```
git add -A
git commit -am "Update README.md"
git push
```

## Авторезация по токену

Settings => Developer settings => Personal access tokens => Generate new token
```
git push https://TOKEN@github.com/username/your-repository.git main
```

# GH
```
$ gh auth login
```
https://github.com/login/device
```
$ gh repo clone kluchnik/<repo-name>
```
```
$ git status
$ git add .
$ git status
$ git commit -m 'create file v1.0'
$ git status
$ git remote set-url origin git@github.com:<user-name>/<repo-name>.git
$ git push
```
```
$ gh release create v1.0
```
