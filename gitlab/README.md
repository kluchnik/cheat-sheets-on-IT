# Пример использования gitlab

## Git global setup:
```
git config --global user.name "name"
git config --global user.email "email"
```

## Create a new repository
```
git clone <URL>.git
cd cd-vminstall
git switch -c main
touch README.md
git add README.md
git commit -m "add README"
git push -u origin main
```

## Push an existing folder
```
cd existing_folder
git init --initial-branch=main
git remote add origin <URL>.git
git add .
git commit -m "Initial commit"
git push -u origin main
```

## Push an existing Git repository
```
cd existing_repo
git remote rename origin old-origin
git remote add origin <URL>.git
git push -u origin --all
git push -u origin --tags
```
