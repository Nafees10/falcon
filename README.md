# Falcon

A simple template engine for the
[zuko programming language](https://github.com/shehryar49/zuko)

## Installing Falcon
You can install this module using zuko's package manager fiza. Zuko must be installed on your system.
### Linux
```bash
fiza update
fiza install falcon
```
### Windows
```
cd C:\zuko
zuko fiza.zu update
zuko fiza.zu install falcon
```
## Building Falcon
If you want to build manually(to contribute perhaps), you can use cmake to build this module.
Zuko Language must be installed prior to installing falcon.

```bash
git clone https://github.com/Nafees10/falcon.git
cd falcon
cmake .
sudo mv falcon.so /opt/zuko/modules
```

## Syntax

Falcon adds several tags to html:

### `<for a b>`

Repeats rendering of whatever is between the `<for ..>` and corresponding
`</for>` tag.

`b` must be a list. Within the opening and closing tags, `a` is available as
an element in the array b.

```html
<for name names>
	<p> {name} </p>
</for>
```

### `<if x>`

Only renders inner content if `x` is defined, and if `x` is a boolean, then only
if it is `true`.

This also works for dictionaries and objects, i.e: if `x` is a dictionary, and
you need to check a key `"foo"`, then `<if x.foo>` will do that.

if `x` is a class instance, and you need to checks a public member `"foo"`, then
`<if x.foo>` will do that.

The `</if>` tag can be followed by an `<else>`, or an `<elseif ..>` tag too.
It does not need to follow immediately, i.e: There can be other tags or static
html in-between a closing.

```html
<if user.isAdmin>
	<a class=button> Delete </a>
	<a class=button> Edit </a>
</if>
Some Stuff in between
<elseif user.isModerator>
	<a class=button> Edit </a>
</elseif>
<else>
	<p> Not Authorized </p>
</else>
```

### `{string.interpolation}`

`{..}` can be used to interpolate strings into rendered output.

`{x}` will look for a key `x` in the provided map, and interpolate it's value,
given that it is a string. It not being a string is an error.

`{x.y}` will work for both cases where `x` is a dictionary, and for the case
where it is a class instance. For dictionary, it will look for a key `"y"`, for
class instance, it will look for a public member named `y`.

```html
<h1> {title} </h1>
<for post posts>
	<h2> {post.title} </h2>
	<p> {post.content} </p>
</for>
```

---

## Using falcon

```zuko
import falcon
var posts = getPostsFromDB()
falcon.render("filename.falcon", {"key" : "value", "posts" : posts})
```

---

## `TODO` for future versions

* Cache parsed template files, to avoid parsing templates on every render
* Add dot operator (similar to one in `if`) to `for`
