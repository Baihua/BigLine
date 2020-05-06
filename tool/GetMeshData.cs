using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;
public class GetMeshData : MonoBehaviour {

	// Use this for initialization
	void Start () {

	}

	// Update is called once per frame
	void Update () {

	}

	[ContextMenu ("getMeshData-暂时只支持一层节点为mesh")]
	public void GetMeshData_ () {
		// MeshFilter mesh = GetComponent<MeshFilter>();
		// foreach(Vector3 v in mesh.sharedMesh.vertices){
		// 	Debug.Log(v);
		// 	Debug.Log(transform.TransformPoint(v));
		// }
		// Debug.Log(mesh.sharedMesh.vertexCount);
		// Debug.Log(mesh.sharedMesh.normals.Length);
		// Debug.Log(mesh.sharedMesh.triangles.Length);
		// for(var i = 0 ; i < mesh.sharedMesh.triangles.Length;)
		// {

		// 	Debug.Log(mesh.sharedMesh.triangles[i]+","+mesh.sharedMesh.triangles[i+1]+","+mesh.sharedMesh.triangles[i+2]);
		// 	i+=3;
		// }
		string info = "";
		MeshFilter[] mfs = GetComponentsInChildren<MeshFilter> ();
		foreach (MeshFilter f in mfs) {
			//
			string line = "";
			if (f.tag == "mesh") {
				line += "mesh ";
				line += f.name + " indices:";
				foreach (int i in f.sharedMesh.triangles) {
					line += " " + i.ToString ();
				}
				line += " point:";
				for (var i = 0; i < f.sharedMesh.vertices.Length; i++) {
					Vector3 p = transform.TransformPoint (f.transform.TransformPoint (f.sharedMesh.vertices[i]));
					line += " " + p.x + " " + p.y + " " + p.z;
				}
			} else if (f.tag == "sphere") {
				line += "sphere ";
				line += f.name + " radius: " + f.transform.localScale.x;
				line += " point: "+f.transform.position.x+" "+ f.transform.position.y+" "+f.transform.position.z;
			}
			info += line+"\n";
		}
		Debug.Log (info);

		System.IO.File.WriteAllText (@"D:\BigLine\" + gameObject.name + ".txt", info);
	}
}